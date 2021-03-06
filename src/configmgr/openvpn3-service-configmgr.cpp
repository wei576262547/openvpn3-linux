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
#include "dbus/path.hpp"
#include "configmgr.hpp"
#include "log/dbus-log.hpp"
#include "common/utils.hpp"

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

    // Enable automatic shutdown if the config manager is
    // idling for 1 minute or more.  By idling, it means
    // no configuration files is stored in memory.
    IdleCheck::Ptr idle_exit = new IdleCheck(main_loop, std::chrono::minutes(3));
    idle_exit->SetPollTime(std::chrono::seconds(30));

    ConfigManagerDBus cfgmgr(G_BUS_TYPE_SYSTEM);
    // cfgmgr.SetLogFile("/tmp/openvpn3-service-configmgr.log");
    cfgmgr.EnableIdleCheck(idle_exit);
    cfgmgr.Setup();

    idle_exit->Enable();
    g_main_loop_run(main_loop);
    g_main_loop_unref(main_loop);
    idle_exit->Disable();
    idle_exit->Join();

    return 0;
}
