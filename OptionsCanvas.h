/*
 * $Id: //devel/DCITU/OptionsCanvas.h#6 $
 *
 * DCITU for OS/2 -- Digital Camera Image Transfer Utility for OS/2
 * Copyright (C) 1997-2000 Stephane Charette, stephanecharette@telus.net
 * Copyright (C) 2000-2002 C Code Run Consulting & Investments, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or at
 * your option, any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#ifndef OptionsCanvas_H
#define OptionsCanvas_H

#define WND_OPTIONS                 1200
#define CNV_MAIN                    1201

#define CNV_OPTIONS_CONNECT         1210
#define CB_CAMERA_TYPE              1211
#define CK_ADVANCED_COM_PORT        1212
#define SB_COM_PORT                 1213
#define EF_COM_PORT                 1214
#define SB_BAUD_RATE                1215
#define SB_PAUSE_BETWEEN_COMMANDS   1216
#define SB_TIMEOUT                  1217
#define CB_EXT_BUFFERS              1218
#define SB_RETRY_PARTIALS           1219
#define SB_BAUD_RATE_PAUSE          1209 // out of sequence #

#define CNV_OPTIONS_CAMERAS         1220
#define CK_IGNORE_MEMORY_CARD       1221
#define CK_CONVERT_KDC_TO_JPG       1222
#define CK_USE_RAW_PATH             1223
#define CK_KEEP_THUMBNAILS          1224
#define CK_ALTERNATE_STARTUP        1225
#define CK_IGNORE_BREAK             1226
#define CK_REMEMBER_NUMBERING       1227
#define TXT_LAST_IMAGE_NUMBER       1228
#define SB_LAST_IMAGE_NUMBER        1229

#define CNV_OPTIONS_PREFERENCES     1230
#define CK_USE_HIGHEST_PRIORITY     1231
#define CK_CONNECT_ON_STARTUP       1232
#define CK_AUTO_DOWNLOAD_THUMBNAILS 1233
#define CK_4_BIT_THUMBNAIL          1234
#define CK_24_BIT_THUMBNAIL         1235
#define CK_AUTO_DOWNLOAD_IMAGES     1236
#define CK_OVERWRITE_IMAGES         1237
#define CK_DELETE_IMAGES            1238

#define CNV_OPTIONS_TRANSFER        1240
#define EF_TRANSFER_DIR             1241
#define CK_DELETE_FAILED_TRANSFERS  1242
#define TXT_TRANSFER_DIR            1243
#define TXT_TRANSFER_DIR_RELATIVE   1244

#define CNV_OPTIONS_REGISTER        1250
#define EF_REGISTER_NAME            1251
#define EF_REGISTER_KEY             1252
#define PB_REGISTER                 1253

#define CNV_OPTIONS_BUTTONS         1260
#define PB_HELP                     1261
#define PB_OK                       1262
#define PB_CANCEL                   1263

#define CNV_OPTIONS_SCRIPT          1270
#define CK_SCRIPT_AFTER_CONNECTION  1271
#define CK_SCRIPT_AFTER_CONNECTION_FG 1272
#define EF_SCRIPT_AFTER_CONNECTION  1273
#define CK_SCRIPT_AFTER_TRANSFER    1274
#define CK_SCRIPT_AFTER_TRANSFER_FG 1275
#define EF_SCRIPT_AFTER_TRANSFER    1276
#define CK_SCRIPT_AFTER_DELETE      1277
#define CK_SCRIPT_AFTER_DELETE_FG   1278
#define EF_SCRIPT_AFTER_DELETE      1279
#define CK_SCRIPT_NO_MORE_IMAGES    1280
#define CK_SCRIPT_NO_MORE_IMAGES_FG 1281
#define EF_SCRIPT_NO_MORE_IMAGES    1282
#define CK_SCRIPT_AFTER_DISCONNECTING 1283
#define CK_SCRIPT_AFTER_DISCONNECTING_FG 1284
#define EF_SCRIPT_AFTER_DISCONNECTING 1285
#define ID_HOW_TO_USE_SCRIPTS       1286  // used in the .IPF file
#define ID_HOW_TO_SET_FG_SCRIPTS    1287  // used in the .IPF file

#define CNV_OPTIONS_SERIAL          1290
#define SB_DTR                      1291
#define CK_CTS                      1292
#define CK_DSR                      1293
#define CK_DCD                      1294
#define CK_INPUT_DSR                1295
#define CK_TRANSMIT_FLOW_CONTROL    1296
#define CK_RECEIVE_FLOW_CONTROL     1297
#define SB_RTS                      1298
#define PB_RESET_SERIAL             1299
#define ID_HOW_TO_SET_SERIAL_LINE   1300  // used in the .IPF file

#endif

