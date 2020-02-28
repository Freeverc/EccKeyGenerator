﻿#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>

using namespace std;
int main()
{
	cout << "\n                   Database retriving application!                  \n" << endl;
	//point_conversion_form_t form = { POINT_CONVERSION_UNCOMPRESSED };

	const int KEY_X_LEN = 32;

	BN_CTX* ctx = BN_CTX_new();
	BN_CTX_start(ctx);

	BIGNUM* p = BN_new();
	BIGNUM* xBase = BN_new();
	BIGNUM* yBase = BN_new();
	BIGNUM* xStart = BN_new();
	BIGNUM* yStart = BN_new();
	BIGNUM* xCurrent = BN_new();
	BIGNUM* yCurrent = BN_new();
	BIGNUM* xEnd = BN_new();
	BIGNUM* yEnd = BN_new();
	BIGNUM* xBuffer = BN_new();
	BIGNUM* kStart = BN_new();
	BIGNUM* kCurrent = BN_new();
	BIGNUM* kEnd = BN_new();
	EC_GROUP* ecg = EC_GROUP_new_by_curve_name(NID_secp256k1);
	EC_POINT* ecpBase = EC_POINT_new(ecg);
	EC_POINT* ecpStart = EC_POINT_new(ecg);
	EC_POINT* ecpCurrent = EC_POINT_new(ecg);
	EC_POINT* ecpEnd = EC_POINT_new(ecg);
	clock_t tStart, tEnd;
	double totaltime;
	int found;
	string s;

	BN_hex2bn(&p, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
	BN_hex2bn(&xBase, "79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798");
	BN_hex2bn(&yBase, "483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8");
	EC_POINT_set_compressed_coordinates(ecg, ecpBase, xBase, 0, ctx);
	cout << "The base point G : " << endl;
	cout << "X coordinate = " << BN_bn2hex(xBase) << endl;
	cout << "Y coordinate = " << BN_bn2hex(yBase) << endl;
	cout << "Uncompressed form : \n" << EC_POINT_point2hex(ecg, ecpBase, POINT_CONVERSION_UNCOMPRESSED, ctx) << endl;

	// input
	//cout << "please input X coordinate of G (in Hex):" << endl;
	//cin >> s;
	//BN_hex2bn(&xBase, s.c_str());
	//cout << "please input Y coordinate of G (in Hex):" << endl;
	//cin >> s;
	//BN_hex2bn(&yBase, s.c_str());
	cout << "please input start value of k (in Hex):" << endl;
	cin >> s;
	BN_hex2bn(&kStart, s.c_str());
	cout << "please input  end  value of k (in Hex):" << endl;
	cin >> s;
	BN_hex2bn(&kEnd, s.c_str());
	if (BN_cmp(kStart, kEnd) == 1)
		BN_swap(kStart, kEnd);

	// gen keys 
	tStart = clock();

	BN_CTX_get(ctx);
	EC_POINT_mul(ecg, ecpStart, NULL, ecpBase, kStart, ctx);
	EC_POINT_mul(ecg, ecpEnd, NULL, ecpBase, kEnd, ctx);
	EC_POINT_get_affine_coordinates(ecg, ecpStart, xStart, yStart, ctx);
	EC_POINT_get_affine_coordinates(ecg, ecpEnd, xEnd, yEnd, ctx);
	xCurrent = BN_dup(xStart);
	yCurrent = BN_dup(yStart);
	kCurrent = BN_dup(kStart);
	EC_POINT_copy(ecpCurrent, ecpStart);
	unsigned char bufKeyX[KEY_X_LEN + 10];
	FILE* dataFile = fopen("publicKeys10000.txt", "rb");
	//int ch = fgetc(dataFile);
	//if (ch == EOF)
	//	return 0;
	FILE* searchFile = fopen("retriveKeys.txt", "wb");
	//cout << "the private key = " << BN_bn2hex(kCurrent) << endl;
	//cout << "x of public key = " << BN_bn2hex(xCurrent) << endl;
	//cout << "y of public key = " << BN_bn2hex(yCurrent) << endl;
	//cout << "Uncompressed form : \n" << EC_POINT_point2hex(ecg, ecpCurrent, POINT_CONVERSION_UNCOMPRESSED, ctx) << endl;
	//s = EC_POINT_point2hex(ecg, ecpCurrent, POINT_CONVERSION_UNCOMPRESSED, ctx);
	//fputs(s.c_str(), dataFile);
	//fwrite(bufKeyX, sizeof(unsigned char), KEY_X_LEN, dataFile);
	//while(BN_cmp(kCurrent, kEnd) < 1)
	BIGNUM* increment = BN_new();
	BN_one(increment);
	long offset = 0;
	int foundNum = 0;
	while(BN_cmp(kCurrent, kEnd)<1)
	{
		fseek(dataFile, offset, SEEK_SET);
		found = 0;
		while (!feof(dataFile))
		{
			fread(bufKeyX, sizeof(unsigned char), KEY_X_LEN, dataFile);
			BN_bin2bn(bufKeyX, KEY_X_LEN, xBuffer);
			//cout<< "buffer :  " << BN_bn2hex(xBuffer) << endl;
			if (BN_cmp(xBuffer, xCurrent) == 0)
			{
				foundNum++;
				found = 1;
				offset = ftell(dataFile);
				fwrite(bufKeyX, sizeof(unsigned char), KEY_X_LEN, searchFile);
				cout << "found "<< endl;
				cout << "the private key = " << BN_bn2hex(kCurrent) << endl;
				cout << "x of public key = " << BN_bn2hex(xCurrent) << endl;
				cout << "y of public key = " << BN_bn2hex(yCurrent) << endl;
				break;
			}
		}
		if (!found)
		{
				cout << "not found" << endl;
		}
		
		BN_CTX_get(ctx);
		EC_POINT_add(ecg, ecpCurrent, ecpCurrent, ecpBase, ctx);
		EC_POINT_get_affine_coordinates(ecg, ecpCurrent, xCurrent, yCurrent, ctx);
		BN_add(kCurrent, kCurrent, increment);
		//EC_POINT_mul(ecg, ecpCurrent, NULL, ecpStart, kCurrent, ctx);
		//fwrite("\n", sizeof(unsigned char), 1, dataFile);
		//fputs(s.c_str(), dataFile);
		//cout << "the private key = " << BN_bn2hex(kCurrent) << endl;
		//cout << "x of public key = " << BN_bn2hex(xCurrent) << endl;
		//cout << "y of public key = " << BN_bn2hex(yCurrent) << endl;
		//cout << "Uncompressed form : \n" << EC_POINT_point2hex(ecg, ecpCurrent, POINT_CONVERSION_UNCOMPRESSED, ctx) << endl;
	}

	BN_CTX_end(ctx);
	BN_free(xStart);
	BN_free(yStart);
	BN_free(xCurrent);
	BN_free(yCurrent);
	BN_free(xEnd);
	BN_free(yEnd);
	BN_free(kStart);
	BN_free(kCurrent);
	BN_free(kEnd);
	EC_GROUP_free(ecg);
	EC_POINT_free(ecpBase);
	EC_POINT_free(ecpStart);
	EC_POINT_free(ecpCurrent);
	EC_POINT_free(ecpEnd);
	tEnd = clock();
	fclose(dataFile);
	fclose(searchFile);
	totaltime = (double)(tEnd - tStart) / CLOCKS_PER_SEC;
	cout << "totaly found : " << foundNum << endl;
	cout << "Database retriving used : " << totaltime << " seconds." << endl;
}
