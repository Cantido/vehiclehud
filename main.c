/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 *  Authors: Robert Richter and Aaron Hall                                 *
 *                                                                         *
 *  Copyright 2013                                                         *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <unistd.h>

#include "obd.h"

int main(int argc, char** argv) {
    int obd_fd = 0;
    int rpm = 0;
    
    obd_fd = serial_connect();
    
    obd_init(obd_fd);
    
    while(1) {
        rpm = get_RPM(obd_fd);
        printf("RPM: %d revolutions/min\n", rpm);
        sleep(2);
    }
    return 0;
}
