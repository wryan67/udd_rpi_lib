##########################################################################
# Makefile
#
# This software is a devLib extension to wiringPi <http://wiringpi.com/>
# and enables it to control an LCD1602 or LCD2004 via a pcf8574 module
# http://ww1.microchip.com/downloads/en/devicedoc/20001952c.pdf
# 
#
# Copyright (c) 2019 Wade Ryan
#
# If you have questions or improvements email me at
# wryan67@gmail.com
#
# This software is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The given code is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You can view the contents of the licence at <http://www.gnu.org/licenses/>.
##########################################################################
# Sorry, I hate make and cmake, but at least I give you a 
# make file interface, so you can use make like you're used to
##########################################################################

all: compile

clean: chmod
	@sudo ./build.sh clean

compile: chmod
	@sudo ./build.sh compile
	
package: chmod
	@sudo ./build.sh package
	
install: chmod
	@sudo ./build.sh install

remove: chmod
	@sudo ./build.sh remove

chmod: 
	@sudo chmod 755 build.sh
     
