/*
 * BM92A_Funcs.h
 *
 *  Created on: Sep 30, 2018
 *      Author: User
 */

#ifndef BM92A_FUNCS_H_
#define BM92A_FUNCS_H_



//BM92A Helper I2C Functions
void BM92Asrc_init();
void BM92Asnk_init();
void BM92A_sink_PDO();
void BM92A_fastSet();
void currentPDO();
void pdo100WMode();
void defaultAllRangeMode();

#endif /* BM92A_FUNCS_H_ */
