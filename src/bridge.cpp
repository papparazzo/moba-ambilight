/*
 *  Project:    moba-ambilight
 *
 *  Version:    1.0.0
 *
 *  Copyright (C) 2016 Stefan Paproth <pappi-@gmx.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/agpl.txt>.
 *
 */

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <unistd.h>
#include <moba-common/helper.h>

#include "bridge.h"

namespace {
    const int table[] = {
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
        11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
        13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
        14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
        16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
        18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19,
        19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22,
        22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
        23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25,
        25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27,
        27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 29,
        29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33,
        33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 38, 38, 38,
        38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
        41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
        43, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 47,
        47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 50, 50, 50, 50, 50,
        50, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 53, 53, 53, 53, 53, 53, 53, 53, 53, 54,
        54, 54, 54, 54, 54, 54, 54, 54, 54, 55, 55, 55, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 57, 57, 57, 57, 57, 57, 57, 57,
        57, 58, 58, 58, 58, 58, 58, 58, 58, 59, 59, 59, 59, 59, 59, 59, 59, 59, 60, 60, 60, 60, 60, 60, 60, 60, 60, 61, 61, 61, 61, 61, 61, 61, 61, 62,
        62, 62, 62, 62, 62, 62, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 64, 64, 64, 64, 64, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 65, 66, 66, 66, 66,
        66, 66, 66, 67, 67, 67, 67, 67, 67, 67, 67, 68, 68, 68, 68, 68, 68, 68, 68, 69, 69, 69, 69, 69, 69, 69, 70, 70, 70, 70, 70, 70, 70, 71, 71, 71,
        71, 71, 71, 71, 71, 72, 72, 72, 72, 72, 72, 72, 73, 73, 73, 73, 73, 73, 73, 74, 74, 74, 74, 74, 74, 74, 75, 75, 75, 75, 75, 75, 75, 76, 76, 76,
        76, 76, 76, 77, 77, 77, 77, 77, 77, 77, 78, 78, 78, 78, 78, 78, 78, 79, 79, 79, 79, 79, 79, 80, 80, 80, 80, 80, 80, 80, 81, 81, 81, 81, 81, 81,
        82, 82, 82, 82, 82, 82, 83, 83, 83, 83, 83, 83, 84, 84, 84, 84, 84, 84, 84, 85, 85, 85, 85, 85, 85, 86, 86, 86, 86, 86, 86, 87, 87, 87, 87, 87,
        87, 88, 88, 88, 88, 88, 89, 89, 89, 89, 89, 89, 90, 90, 90, 90, 90, 90, 91, 91, 91, 91, 91, 91, 92, 92, 92, 92, 92, 93, 93, 93, 93, 93, 93, 94,
        94, 94, 94, 94, 95, 95, 95, 95, 95, 95, 96, 96, 96, 96, 96, 97, 97, 97, 97, 97, 98, 98, 98, 98, 98, 98, 99, 99, 99, 99, 99, 100, 100, 100, 100,
        100, 101, 101, 101, 101, 101, 102, 102, 102, 102, 102, 103, 103, 103, 103, 103, 104, 104, 104, 104, 104, 105, 105, 105, 105, 105, 106, 106, 106, 106,
        106, 107, 107, 107, 107, 108, 108, 108, 108, 108, 109, 109, 109, 109, 109, 110, 110, 110, 110, 110, 111, 111, 111, 111, 112, 112, 112, 112, 112, 113,
        113, 113, 113, 114, 114, 114, 114, 114, 115, 115, 115, 115, 116, 116, 116, 116, 116, 117, 117, 117, 117, 118, 118, 118, 118, 118, 119, 119, 119, 119,
        120, 120, 120, 120, 121, 121, 121, 121, 122, 122, 122, 122, 122, 123, 123, 123, 123, 124, 124, 124, 124, 125, 125, 125, 125, 126, 126, 126, 126, 127,
        127, 127, 127, 128, 128, 128, 128, 129, 129, 129, 129, 130, 130, 130, 130, 131, 131, 131, 131, 132, 132, 132, 132, 133, 133, 133, 133, 134, 134, 134,
        134, 135, 135, 135, 135, 136, 136, 136, 137, 137, 137, 137, 138, 138, 138, 138, 139, 139, 139, 139, 140, 140, 140, 141, 141, 141, 141, 142, 142, 142,
        142, 143, 143, 143, 144, 144, 144, 144, 145, 145, 145, 146, 146, 146, 146, 147, 147, 147, 148, 148, 148, 148, 149, 149, 149, 150, 150, 150, 150, 151,
        151, 151, 152, 152, 152, 153, 153, 153, 153, 154, 154, 154, 155, 155, 155, 156, 156, 156, 156, 157, 157, 157, 158, 158, 158, 159, 159, 159, 159, 160,
        160, 160, 161, 161, 161, 162, 162, 162, 163, 163, 163, 164, 164, 164, 165, 165, 165, 165, 166, 166, 166, 167, 167, 167, 168, 168, 168, 169, 169, 169,
        170, 170, 170, 171, 171, 171, 172, 172, 172, 173, 173, 173, 174, 174, 174, 175, 175, 175, 176, 176, 176, 177, 177, 177, 178, 178, 179, 179, 179, 180,
        180, 180, 181, 181, 181, 182, 182, 182, 183, 183, 183, 184, 184, 185, 185, 185, 186, 186, 186, 187, 187, 187, 188, 188, 189, 189, 189, 190, 190, 190,
        191, 191, 191, 192, 192, 193, 193, 193, 194, 194, 194, 195, 195, 196, 196, 196, 197, 197, 198, 198, 198, 199, 199, 199, 200, 200, 201, 201, 201, 202,
        202, 203, 203, 203, 204, 204, 205, 205, 205, 206, 206, 207, 207, 207, 208, 208, 209, 209, 209, 210, 210, 211, 211, 211, 212, 212, 213, 213, 213, 214,
        214, 215, 215, 216, 216, 216, 217, 217, 218, 218, 219, 219, 219, 220, 220, 221, 221, 222, 222, 222, 223, 223, 224, 224, 225, 225, 225, 226, 226, 227,
        227, 228, 228, 229, 229, 229, 230, 230, 231, 231, 232, 232, 233, 233, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238, 238, 239, 239, 240, 240,
        241, 241, 242, 242, 243, 243, 244, 244, 245, 245, 246, 246, 246, 247, 247, 248, 248, 249, 249, 250, 250, 251, 251, 252, 252, 253, 253, 254, 254, 255,
        255, 256, 256, 257, 257, 258, 258, 259, 259, 260, 260, 261, 261, 262, 262, 263, 263, 264, 264, 265, 265, 266, 266, 267, 267, 268, 268, 269, 270, 270,
        271, 271, 272, 272, 273, 273, 274, 274, 275, 275, 276, 276, 277, 278, 278, 279, 279, 280, 280, 281, 281, 282, 282, 283, 284, 284, 285, 285, 286, 286,
        287, 287, 288, 289, 289, 290, 290, 291, 291, 292, 292, 293, 294, 294, 295, 295, 296, 296, 297, 298, 298, 299, 299, 300, 301, 301, 302, 302, 303, 303,
        304, 305, 305, 306, 306, 307, 308, 308, 309, 309, 310, 311, 311, 312, 312, 313, 314, 314, 315, 315, 316, 317, 317, 318, 319, 319, 320, 320, 321, 322,
        322, 323, 324, 324, 325, 325, 326, 327, 327, 328, 329, 329, 330, 331, 331, 332, 332, 333, 334, 334, 335, 336, 336, 337, 338, 338, 339, 340, 340, 341,
        342, 342, 343, 344, 344, 345, 346, 346, 347, 348, 348, 349, 350, 350, 351, 352, 352, 353, 354, 355, 355, 356, 357, 357, 358, 359, 359, 360, 361, 361,
        362, 363, 364, 364, 365, 366, 366, 367, 368, 369, 369, 370, 371, 371, 372, 373, 374, 374, 375, 376, 377, 377, 378, 379, 379, 380, 381, 382, 382, 383,
        384, 385, 385, 386, 387, 388, 388, 389, 390, 391, 391, 392, 393, 394, 395, 395, 396, 397, 398, 398, 399, 400, 401, 401, 402, 403, 404, 405, 405, 406,
        407, 408, 409, 409, 410, 411, 412, 413, 413, 414, 415, 416, 417, 417, 418, 419, 420, 421, 421, 422, 423, 424, 425, 426, 426, 427, 428, 429, 430, 431,
        431, 432, 433, 434, 435, 436, 436, 437, 438, 439, 440, 441, 442, 442, 443, 444, 445, 446, 447, 448, 449, 449, 450, 451, 452, 453, 454, 455, 456, 456,
        457, 458, 459, 460, 461, 462, 463, 464, 464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 475, 476, 477, 478, 479, 480, 481, 482, 483, 484,
        485, 486, 487, 488, 489, 489, 490, 491, 492, 493, 494, 495, 496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512, 513,
        514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527, 528, 529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 540, 542, 543, 544,
        545, 546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 558, 559, 560, 561, 562, 563, 564, 565, 566, 567, 569, 570, 571, 572, 573, 574, 575, 576,
        577, 579, 580, 581, 582, 583, 584, 585, 586, 588, 589, 590, 591, 592, 593, 594, 596, 597, 598, 599, 600, 601, 603, 604, 605, 606, 607, 609, 610, 611,
        612, 613, 614, 616, 617, 618, 619, 620, 622, 623, 624, 625, 627, 628, 629, 630, 631, 633, 634, 635, 636, 638, 639, 640, 641, 643, 644, 645, 646, 648,
        649, 650, 651, 653, 654, 655, 656, 658, 659, 660, 662, 663, 664, 665, 667, 668, 669, 671, 672, 673, 675, 676, 677, 678, 680, 681, 682, 684, 685, 686,
        688, 689, 690, 692, 693, 694, 696, 697, 699, 700, 701, 703, 704, 705, 707, 708, 709, 711, 712, 714, 715, 716, 718, 719, 721, 722, 723, 725, 726, 728,
        729, 730, 732, 733, 735, 736, 738, 739, 740, 742, 743, 745, 746, 748, 749, 751, 752, 754, 755, 756, 758, 759, 761, 762, 764, 765, 767, 768, 770, 771,
        773, 774, 776, 777, 779, 780, 782, 783, 785, 786, 788, 790, 791, 793, 794, 796, 797, 799, 800, 802, 803, 805, 807, 808, 810, 811, 813, 814, 816, 818,
        819, 821, 822, 824, 826, 827, 829, 830, 832, 834, 835, 837, 839, 840, 842, 843, 845, 847, 848, 850, 852, 853, 855, 857, 858, 860, 862, 863, 865, 867,
        868, 870, 872, 873, 875, 877, 879, 880, 882, 884, 885, 887, 889, 891, 892, 894, 896, 898, 899, 901, 903, 905, 906, 908, 910, 912, 913, 915, 917, 919,
        921, 922, 924, 926, 928, 930, 931, 933, 935, 937, 939, 940, 942, 944, 946, 948, 950, 951, 953, 955, 957, 959, 961, 963, 965, 966, 968, 970, 972, 974,
        976, 978, 980, 982, 983, 985, 987, 989, 991, 993, 995, 997, 999, 1001, 1003, 1005, 1007, 1009, 1011, 1013, 1014, 1016, 1018, 1020, 1022, 1024, 1026,
        1028, 1030, 1032, 1034, 1036, 1038, 1040, 1042, 1045, 1047, 1049, 1051, 1053, 1055, 1057, 1059, 1061, 1063, 1065, 1067, 1069, 1071, 1073, 1075, 1077,
        1080, 1082, 1084, 1086, 1088, 1090, 1092, 1094, 1097, 1099, 1101, 1103, 1105, 1107, 1109, 1112, 1114, 1116, 1118, 1120, 1122, 1125, 1127, 1129, 1131,
        1133, 1136, 1138, 1140, 1142, 1144, 1147, 1149, 1151, 1153, 1156, 1158, 1160, 1162, 1165, 1167, 1169, 1171, 1174, 1176, 1178, 1181, 1183, 1185, 1187,
        1190, 1192, 1194, 1197, 1199, 1201, 1204, 1206, 1208, 1211, 1213, 1215, 1218, 1220, 1223, 1225, 1227, 1230, 1232, 1235, 1237, 1239, 1242, 1244, 1247,
        1249, 1251, 1254, 1256, 1259, 1261, 1264, 1266, 1269, 1271, 1274, 1276, 1278, 1281, 1283, 1286, 1288, 1291, 1293, 1296, 1298, 1301, 1304, 1306, 1309,
        1311, 1314, 1316, 1319, 1321, 1324, 1327, 1329, 1332, 1334, 1337, 1340, 1342, 1345, 1347, 1350, 1353, 1355, 1358, 1360, 1363, 1366, 1368, 1371, 1374,
        1376, 1379, 1382, 1384, 1387, 1390, 1393, 1395, 1398, 1401, 1403, 1406, 1409, 1412, 1414, 1417, 1420, 1423, 1425, 1428, 1431, 1434, 1437, 1439, 1442,
        1445, 1448, 1451, 1453, 1456, 1459, 1462, 1465, 1468, 1470, 1473, 1476, 1479, 1482, 1485, 1488, 1491, 1494, 1496, 1499, 1502, 1505, 1508, 1511, 1514,
        1517, 1520, 1523, 1526, 1529, 1532, 1535, 1538, 1541, 1544, 1547, 1550, 1553, 1556, 1559, 1562, 1565, 1568, 1571, 1574, 1577, 1580, 1583, 1586, 1589,
        1592, 1596, 1599, 1602, 1605, 1608, 1611, 1614, 1617, 1621, 1624, 1627, 1630, 1633, 1636, 1640, 1643, 1646, 1649, 1652, 1656, 1659, 1662, 1665, 1668,
        1672, 1675, 1678, 1681, 1685, 1688, 1691, 1695, 1698, 1701, 1705, 1708, 1711, 1714, 1718, 1721, 1725, 1728, 1731, 1735, 1738, 1741, 1745, 1748, 1752,
        1755, 1758, 1762, 1765, 1769, 1772, 1776, 1779, 1782, 1786, 1789, 1793, 1796, 1800, 1803, 1807, 1810, 1814, 1817, 1821, 1824, 1828, 1832, 1835, 1839,
        1842, 1846, 1849, 1853, 1857, 1860, 1864, 1868, 1871, 1875, 1878, 1882, 1886, 1889, 1893, 1897, 1900, 1904, 1908, 1912, 1915, 1919, 1923, 1927, 1930,
        1934, 1938, 1942, 1945, 1949, 1953, 1957, 1961, 1964, 1968, 1972, 1976, 1980, 1984, 1987, 1991, 1995, 1999, 2003, 2007, 2011, 2015, 2019, 2022, 2026,
        2030, 2034, 2038, 2042, 2046, 2050, 2054, 2058, 2062, 2066, 2070, 2074, 2078, 2082, 2086, 2090, 2094, 2099, 2103, 2107, 2111, 2115, 2119, 2123, 2127,
        2131, 2136, 2140, 2144, 2148, 2152, 2156, 2161, 2165, 2169, 2173, 2177, 2182, 2186, 2190, 2194, 2199, 2203, 2207, 2212, 2216, 2220, 2225, 2229, 2233,
        2238, 2242, 2246, 2251, 2255, 2259, 2264, 2268, 2273, 2277, 2281, 2286, 2290, 2295, 2299, 2304, 2308, 2313, 2317, 2322, 2326, 2331, 2335, 2340, 2344,
        2349, 2353, 2358, 2363, 2367, 2372, 2376, 2381, 2386, 2390, 2395, 2400, 2404, 2409, 2414, 2418, 2423, 2428, 2433, 2437, 2442, 2447, 2452, 2456, 2461,
        2466, 2471, 2475, 2480, 2485, 2490, 2495, 2500, 2504, 2509, 2514, 2519, 2524, 2529, 2534, 2539, 2544, 2549, 2554, 2559, 2564, 2569, 2574, 2579, 2584,
        2589, 2594, 2599, 2604, 2609, 2614, 2619, 2624, 2629, 2634, 2639, 2645, 2650, 2655, 2660, 2665, 2670, 2676, 2681, 2686, 2691, 2696, 2702, 2707, 2712,
        2717, 2723, 2728, 2733, 2739, 2744, 2749, 2755, 2760, 2765, 2771, 2776, 2782, 2787, 2792, 2798, 2803, 2809, 2814, 2820, 2825, 2831, 2836, 2842, 2847,
        2853, 2858, 2864, 2869, 2875, 2881, 2886, 2892, 2897, 2903, 2909, 2914, 2920, 2926, 2931, 2937, 2943, 2949, 2954, 2960, 2966, 2972, 2977, 2983, 2989,
        2995, 3001, 3006, 3012, 3018, 3024, 3030, 3036, 3042, 3048, 3054, 3060, 3065, 3071, 3077, 3083, 3089, 3095, 3101, 3107, 3113, 3120, 3126, 3132, 3138,
        3144, 3150, 3156, 3162, 3168, 3175, 3181, 3187, 3193, 3199, 3206, 3212, 3218, 3224, 3231, 3237, 3243, 3249, 3256, 3262, 3268, 3275, 3281, 3288, 3294,
        3300, 3307, 3313, 3320, 3326, 3333, 3339, 3346, 3352, 3359, 3365, 3372, 3378, 3385, 3391, 3398, 3405, 3411, 3418, 3425, 3431, 3438, 3445, 3451, 3458,
        3465, 3471, 3478, 3485, 3492, 3499, 3505, 3512, 3519, 3526, 3533, 3540, 3546, 3553, 3560, 3567, 3574, 3581, 3588, 3595, 3602, 3609, 3616, 3623, 3630,
        3637, 3644, 3651, 3658, 3666, 3673, 3680, 3687, 3694, 3701, 3709, 3716, 3723, 3730, 3738, 3745, 3752, 3759, 3767, 3774, 3781, 3789, 3796, 3803, 3811,
        3818, 3826, 3833, 3841, 3848, 3856, 3863, 3871, 3878, 3886, 3893, 3901, 3908, 3916, 3924, 3931, 3939, 3947, 3954, 3962, 3970, 3977, 3985, 3993, 4001,
        4008, 4016, 4024, 4032, 4040, 4048, 4055, 4063, 4071, 4079, 4087, 4095
    };
}

Bridge::Bridge(int address) {
    fd = wiringPiI2CSetup(address);
    if(fd == -1) {
        throw BridgeException(moba::common::getErrno("wiringPiI2CSetup"));
    }
    setAllOff();

    wiringPiI2CWriteReg8(fd, Bridge::MODE2, Bridge::OUTDRV);
    wiringPiI2CWriteReg8(fd, Bridge::MODE1, Bridge::ALLCALL);
    usleep(5000);
    int mode1 = wiringPiI2CReadReg8(fd, Bridge::MODE1);
    mode1 = mode1 & ~Bridge::SLEEP;
    wiringPiI2CWriteReg8(fd, Bridge::MODE1, mode1);
    usleep(5000);
}

Bridge::~Bridge() {
    close(fd);
}

void Bridge::softwareReset() {
    int fd = wiringPiI2CSetup(0x00);
    if(fd == -1) {
        throw BridgeException(moba::common::getErrno("wiringPiI2CSetup"));
    }

    // Sends a software reset (SWRST) command to all the drivers on the bus
    if(wiringPiI2CWrite(fd, 0x06) == -1) {
        throw BridgeException(moba::common::getErrno("wiringPiI2CWrite"));
    }

    if(close(fd) == -1) {
        throw BridgeException(moba::common::getErrno("close"));
    }
}

void Bridge::setPWMFrequency(int freq) {
    float oscClock = 25000000.0; // 25MHz
    oscClock /= 4096.0;          // 12-bit
    oscClock /= (float)freq;
    oscClock -= 1.0;

    int prescale = (int)oscClock + 0.5;

    int oldmode = wiringPiI2CReadReg8(fd, Bridge::MODE1);
    int newmode = (oldmode & 0x7F) | 0x10;
    wiringPiI2CWriteReg8(fd, Bridge::MODE1, newmode);
    wiringPiI2CWriteReg8(fd, Bridge::PRESCALE, prescale);
    wiringPiI2CWriteReg8(fd, Bridge::MODE1, oldmode);
    usleep(5000);
    wiringPiI2CWriteReg8(fd, Bridge::MODE1, oldmode | 0x80);
}

void Bridge::setOn(int color) {
    for(int i = 0; i < BANK_COUNT; ++i) {
        setData(color, i, 4096, 0);
    }
}

void Bridge::setOff(int color) {
    for(int i = 0; i < BANK_COUNT; ++i) {
        setData(color, i, 0, 0);
    }
}

void Bridge::setPWM(int color, int on, int off) {
    for(int i = 0; i < BANK_COUNT; ++i) {
        setData(color, i, on, off);
    }
}

void Bridge::setPWMlg(int color, int val) {
    for(int i = 0; i < BANK_COUNT; ++i) {
        setPWMlg(color, i, val);
    }
}

void Bridge::setPWMlg(int color, int bank, int val) {
    setData(color, bank, 0, table[val]);
}

void Bridge::setPWMlg(const BankColorValues &values, int bank) {
    setPWMlg(BankColorValues::WHITE, bank, values.getValue(bank, BankColorValues::WHITE));
    setPWMlg(BankColorValues::RED, bank, values.getValue(bank, BankColorValues::RED));
    setPWMlg(BankColorValues::GREEN, bank, values.getValue(bank, BankColorValues::GREEN));
    setPWMlg(BankColorValues::BLUE, bank, values.getValue(bank, BankColorValues::BLUE));
}

void Bridge::setPWMlg(const BankColorValues &values) {
    for(int i = 0; i < BANK_COUNT; ++i) {
        setPWMlg(values, i);
    }
}

void Bridge::setData(int color, int bank, int on, int off) {
    int channel = (bank * 4 + color) * 4;

    wiringPiI2CWriteReg8(fd, Bridge::LED0_ON_L + channel, on & 0xFF);
    wiringPiI2CWriteReg8(fd, Bridge::LED0_ON_H + channel, on >> 8);
    wiringPiI2CWriteReg8(fd, Bridge::LED0_OFF_L + channel, off & 0xFF);
    wiringPiI2CWriteReg8(fd, Bridge::LED0_OFF_H + channel, off >> 8);
}

void Bridge::setAllOff() {
    wiringPiI2CWriteReg8(fd, Bridge::ALL_LED_ON_L, 0);
    wiringPiI2CWriteReg8(fd, Bridge::ALL_LED_ON_H, 0);
    wiringPiI2CWriteReg8(fd, Bridge::ALL_LED_OFF_L, 0);
    wiringPiI2CWriteReg8(fd, Bridge::ALL_LED_OFF_H, 0);
}
