//  OpenVPN 3 Linux client -- Next generation OpenVPN client
//
//  Copyright (C) 2017      OpenVPN Inc. <sales@openvpn.net>
//  Copyright (C) 2017      David Sommerseth <davids@openvpn.net>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, version 3 of the
//  License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include "dbus/core.hpp"
#include "sessionmgr.hpp"
#include "log/dbus-log.hpp"

using namespace openvpn;

int main(int argc, char **argv)
{
    std::cout << get_version(argv[0]) << std::endl;

    // This program does not require root privileges,
    // so if used - drop those privileges
    drop_root();

    GMainLoop *main_loop = g_main_loop_new(NULL, FALSE);
    g_unix_signal_add(SIGINT, stop_handler, main_loop);
    g_unix_signal_add(SIGTERM, stop_handler, main_loop);

    IdleCheck::Ptr idle_exit = new IdleCheck(main_loop, std::chrono::minutes(3));
    idle_exit->SetPollTime(std::chrono::seconds(30));

    SessionManagerDBus sessmgr(G_BUS_TYPE_SYSTEM);
    // sessmgr.SetLogFile("/tmp/openvpn3-service-sessionmgr.log");
    sessmgr.EnableIdleCheck(idle_exit);
    sessmgr.Setup();

    idle_exit->Enable();
    g_main_loop_run(main_loop);
    g_main_loop_unref(main_loop);
    idle_exit->Disable();
    idle_exit->Join();

    return 0;
}
