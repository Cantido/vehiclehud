/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  This file is a part of vehiclehud
 *  
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <config.h>
#include "obd.h"

#include "elm327.h"
#define GET_OBD_DATA(x, y) get_elm327_data(x, y)

int get_RPM(int obd_fd) {
    return (GET_OBD_DATA(obd_fd, PID_RPM) / 4);
}
