// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek Crypto test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/scatterlist.h>
#include <crypto/hash.h>
#include <crypto/skcipher.h>


#define NUM_TEST_THREADS	2

struct rtk_crypto_test_t {
	struct task_struct task[NUM_TEST_THREADS];
	int thread_num;
};

static struct rtk_crypto_test_t rtk_crypto_test;

static int rtk_md5_test(void)
{
	char message[] = "The quick brown fox jumps over the lazy dog";
	u8 md5_digest[] = "\x9e\x10\x7d\x9d\x37\x2b\xb6\x82\x6b\xd8\x1d\x35\x42\xa4\x19\xd6";
	int ret;
	struct scatterlist sg;
	u8 result[16];
	struct crypto_ahash *tfm;
	struct ahash_request *req;
	unsigned int msglen = 43;

	tfm = crypto_alloc_ahash("md5", 0, 0);
	if (IS_ERR(tfm)) {
		pr_err("Error: Failed to allocate transform for md5: %ld\n", PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}
	pr_info("Allocated driver name: %s", crypto_hash_alg_common(tfm)->base.cra_driver_name);

	req = ahash_request_alloc(tfm, GFP_ATOMIC);
	if (!req) {
		pr_err("Error: Failed to allocate request for md5\n");
		ret = -ENOMEM;
		goto fail;
	}

	ahash_request_set_callback(req, 0, NULL, NULL);
	sg_init_one(&sg, message, msglen);
	ahash_request_set_crypt(req, &sg, result, msglen);

	ret = crypto_ahash_digest(req);
	if (ret) {
		pr_err("Error: Failed to do digest for md5\n");
		goto fail;
	}

	if (memcmp(result, md5_digest, 16) != 0) {
		ret = -EINVAL;
	} else {
		ret = 0;
	}

fail:
	ahash_request_free(req);
	crypto_free_ahash(tfm);
	return ret;
}

static int rtk_sha256_test(void)
{
	char message[] = "abcdefg";
	u8 digest[] = "\x7D\x1A\x54\x12\x7B\x22\x25\x02\xF5\xB7\x9B\x5F\xB0\x80\x30\x61"
				  "\x15\x2A\x44\xF9\x2B\x37\xE2\x3C\x65\x27\xBA\xF6\x65\xD4\xDA\x9A";

	int ret;
	struct scatterlist sg;
	u8 result[32];
	struct crypto_ahash *tfm;
	struct ahash_request *req;
	unsigned int msglen = 7;

	tfm = crypto_alloc_ahash("sha256", 0, 0);
	if (IS_ERR(tfm)) {
		pr_err("Error: Failed to allocate transform for sha256: %ld\n", PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}
	pr_info("Allocated driver name: %s", crypto_hash_alg_common(tfm)->base.cra_driver_name);

	req = ahash_request_alloc(tfm, GFP_ATOMIC);
	if (!req) {
		pr_err("Error: Failed to allocate request for sha256\n");
		ret = -ENOMEM;
		goto fail;
	}

	ahash_request_set_callback(req, 0, NULL, NULL);
	sg_init_one(&sg, message, msglen);
	ahash_request_set_crypt(req, &sg, result, msglen);

	ret = crypto_ahash_digest(req);
	if (ret) {
		pr_err("Error: Failed to do digest for sha256\n");
		goto fail;
	}

	if (memcmp(result, digest, 32) != 0) {
		ret = -EINVAL;
	} else {
		ret = 0;
	}

fail:
	ahash_request_free(req);
	crypto_free_ahash(tfm);
	return ret;
}

static int rtk_sha512_test(void)
{
	u8 message[] = "\x0f\xda\x56\x1a\x2b\x3b\xac\xf0\xd7\x47\xf3\x75\x12\x54\xe9\xe9"
				   "\x9c\x38\xbe\xf1\x70\xdb\xed\xe5\x4e\x68\x13\x8b\xe7\xbb\xd5\xa8"
				   "\xcb\xde\xb9\x6d\x5a\xf6\x0e\xad\xe8\x00\x1b\x67\xd4\xc3\xe9\x2f"
				   "\x40\xe0\xf5\xf3\xa8\xe1\x22\xfa\xc9\x4b\x6d\xf0\x1f\x3a\x97\xe1"
				   "\x19\x49\x7a\xc0\xfa\x40\x85\xe7\x1f\x34\xaf\x93\xb8\x78\xc6\x2c"
				   "\x49\xaa\x9f\x85\x7c\x4d\x81\x8f\xfa\x96\x1c\x97\x09\x62\xc2\xa3"
				   "\x2e\x88\xa1\x0e\x6d\xf2\x0a\xd3\x0e\x36\xe6\x4a\x67\x18\x8e\xd3"
				   "\xe3\x63\x73\x30\x73\x5b\x84\x18\xa3\x15\xef\xe2\x60\x04\x4d\xeb";
	u8 digest[] = "\xA1\xF8\x80\x79\x13\xA3\x2C\x7B\xD8\x89\x05\x9C\xB0\xBC\x14\x02"
				  "\x87\x49\x02\x4D\x84\x4B\x45\x4C\x7D\x39\xF4\xBC\x83\x9A\xF8\xA7"
				  "\x1A\xB5\x3B\x33\x5C\x48\x44\xEB\x62\x9E\xF0\x7C\xC5\x4A\xBF\xA5"
				  "\x84\xB3\x38\x50\xC2\xD5\xD3\x83\x26\xFE\xFE\x7F\x2B\xB0\xF8\x33";

	int ret;
	struct scatterlist sg;
	u8 result[64];
	struct crypto_ahash *tfm;
	struct ahash_request *req;
	unsigned int msglen = 128;

	tfm = crypto_alloc_ahash("sha512", 0, 0);
	if (IS_ERR(tfm)) {
		pr_err("Error: Failed to allocate transform for sha512: %ld\n", PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}
	pr_info("Allocated driver name: %s", crypto_hash_alg_common(tfm)->base.cra_driver_name);

	req = ahash_request_alloc(tfm, GFP_ATOMIC);
	if (!req) {
		pr_err("Error: Failed to allocate request for sha512\n");
		ret = -ENOMEM;
		goto fail;
	}

	ahash_request_set_callback(req, 0, NULL, NULL);
	sg_init_one(&sg, message, msglen);
	ahash_request_set_crypt(req, &sg, result, msglen);

	ret = crypto_ahash_digest(req);
	if (ret) {
		pr_err("Error: Failed to do digest for sha512\n");
		goto fail;
	}

	if (memcmp(result, digest, 64) != 0) {
		ret = -EINVAL;
	} else {
		ret = 0;
	}

fail:
	ahash_request_free(req);
	crypto_free_ahash(tfm);
	return ret;
}

static int rtk_hmac_sha512_test(void)
{
	char message[] = "The quick brown fox jumps over the lazy dog";
	char key[] = "key";
	u8 digest[] = "\xb4\x2a\xf0\x90\x57\xba\xc1\xe2\xd4\x17\x08\xe4\x8a\x90\x2e\x09"
				  "\xb5\xff\x7f\x12\xab\x42\x8a\x4f\xe8\x66\x53\xc7\x3d\xd2\x48\xfb"
				  "\x82\xf9\x48\xa5\x49\xf7\xb7\x91\xa5\xb4\x19\x15\xee\x4d\x1e\xc3"
				  "\x93\x53\x57\xe4\xe2\x31\x72\x50\xd0\x37\x2a\xfa\x2e\xbe\xeb\x3a";
	int ret;
	struct scatterlist sg;
	u8 result[64];
	struct crypto_ahash *tfm;
	struct ahash_request *req;
	unsigned int keylen = 3;
	unsigned int msglen = 43;

	tfm = crypto_alloc_ahash("hmac(sha512)", 0, 0);
	if (IS_ERR(tfm)) {
		pr_err("Error: Failed to allocate transform for hmac-sha512: %ld\n", PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}
	pr_info("Allocated driver name: %s", crypto_hash_alg_common(tfm)->base.cra_driver_name);

	req = ahash_request_alloc(tfm, GFP_ATOMIC);
	if (!req) {
		pr_err("Error: Failed to allocate request for hmac-sha512\n");
		ret = -ENOMEM;
		goto fail;
	}

	ret = crypto_ahash_setkey(tfm, key, keylen);
	if (ret) {
		pr_err("Error: Failed to setkey for hmac-sha512\n");
		goto fail;
	}

	ahash_request_set_callback(req, 0, NULL, NULL);
	sg_init_one(&sg, message, msglen);
	ahash_request_set_crypt(req, &sg, result, msglen);

	ret = crypto_ahash_digest(req);
	if (ret) {
		pr_err("Error: Failed to do digest for hmac-sha512\n");
		goto fail;
	}

	if (memcmp(result, digest, 64) != 0) {
		ret = -EINVAL;
	} else {
		ret = 0;
	}

fail:
	ahash_request_free(req);
	crypto_free_ahash(tfm);
	return ret;
}

static int rtk_aes_test(void)
{
	char plaintext[] = "\x6b\xc1\xbe\xe2\x2e\x40\x9f\x96\xe9\x3d\x7e\x11\x73\x93\x17\x2a";
	char key[] = "\x60\x3d\xeb\x10\x15\xca\x71\xbe\x2b\x73\xae\xf0\x85\x7d\x77\x81"
				 "\x1f\x35\x2c\x07\x3b\x61\x08\xd7\x2d\x98\x10\xa3\x09\x14\xdf\xf4";
	char iv[] = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F";
	char ciphertext[16];
	char decrypttext[16];
	int ret;
	struct scatterlist sg[2];
	struct scatterlist sg2[2];
	struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	unsigned int plen = 16;
	unsigned int keylen = 32;
	int i;

	tfm = crypto_alloc_skcipher("cbc(aes)", 0, 0);
	if (IS_ERR(tfm)) {
		pr_err("Error: Failed to allocate transform for aes-cbc: %ld\n", PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}

	req = skcipher_request_alloc(tfm, GFP_ATOMIC);
	if (!req) {
		pr_err("Error: Failed to allocate request for aes-cbc\n");
		ret = -ENOMEM;
		goto fail;
	}

	crypto_skcipher_clear_flags(tfm, CRYPTO_TFM_REQ_FORBID_WEAK_KEYS);
	ret = crypto_skcipher_setkey(tfm, key, keylen);
	if (ret) {
		pr_err("Error: Failed to setkey for aes-cbc\n");
		goto fail;
	}

	skcipher_request_set_callback(req, 0, NULL, NULL);
	sg_init_one(&sg[0], plaintext, plen);
	sg_init_one(&sg[1], ciphertext, plen);
	skcipher_request_set_crypt(req, &sg[0], &sg[1], plen, iv);

	ret = crypto_skcipher_encrypt(req);
	if (ret) {
		pr_err("Error: Failed to encrypt for aes-cbc\n");
		goto fail;
	}

	for (i = 0; i < plen; i++) {
		pr_debug("Dump ciphertext[%d]: 0x%04X\n", i, ciphertext[i]);
	}

	sg_init_one(&sg2[0], ciphertext, plen);
	sg_init_one(&sg2[1], decrypttext, plen);
	crypto_skcipher_clear_flags(tfm, CRYPTO_TFM_REQ_FORBID_WEAK_KEYS);
	skcipher_request_set_crypt(req, &sg2[0], &sg2[1], plen, iv);

	ret = crypto_skcipher_decrypt(req);
	if (ret) {
		pr_err("Error: Failed to encrypt for aes-cbc\n");
		goto fail;
	}

	if (memcmp(plaintext, decrypttext, plen) != 0) {
		ret = -EINVAL;
	} else {
		ret = 0;
	}

fail:
	skcipher_request_free(req);
	crypto_free_skcipher(tfm);
	return ret;
}

static int rtk_crypto_test_inst_init(void *id)
{
	static int thread_num = 0;
	int result = 0;
	int ret;

	pr_info("\n##### Crypto test thread instance %d #####\n", ++thread_num);
	ret = rtk_sha256_test();
	if (ret) {
		result = 1;
		pr_err("SHA256 test fail\n");
	} else {
		pr_info("SHA256 test success\n");
	}

	ret = rtk_sha512_test();
	if (ret) {
		result = 1;
		pr_err("SHA512 test fail\n");
	} else {
		pr_info("SHA512 test success\n");
	}

	ret = rtk_md5_test();
	if (ret) {
		result = 1;
		pr_err("MD5 test fail\n");
	} else {
		pr_info("MD5 test success\n");
	}

	ret = rtk_hmac_sha512_test();
	if (ret) {
		result = 1;
		pr_err("HMAC-SHA512 test fail\n");
	} else {
		pr_info("HMAC-SHA512 test success\n");
	}

	ret = rtk_aes_test();
	if (ret) {
		result = 1;
		pr_err("AES CBC test fail\n");
	} else {
		pr_info("AES CBC test success\n");
	}

	while (!kthread_should_stop()) {
		if (result) {
			pr_info("Crypto test fail\n");
		} else {
			pr_info("Crypto test pass\n");
		}

		if (thread_num == NUM_TEST_THREADS) {
			break;
		}

		schedule_timeout_interruptible(HZ);
	}

	return 0;
}


static int rtk_crypto_test_init(void)
{
	static struct task_struct *ptrtask;
	int i;

	pr_info("Creating %d crypto test threads\n", NUM_TEST_THREADS);

	memset(&rtk_crypto_test, 0, sizeof(rtk_crypto_test));
	for (i = 0; i < NUM_TEST_THREADS; i++) {
		rtk_crypto_test.thread_num = i;
		ptrtask = &rtk_crypto_test.task[i];
		ptrtask = kthread_run(rtk_crypto_test_inst_init, (void *)&rtk_crypto_test.thread_num, "crypto-test-kthread");

		/*
		 * Note: We can remove this sleep to ensure even more simultaneous multithread access.
		 * It (crypto driver) still works ok, but then sometimes the test application data
		 * gets overwritten when number of threads is very large for e.g. 10
		 */
		//msleep(10);
	}

	return 0;
}

static void rtk_crypto_test_exit(void)
{
	pr_info("Crypto test exit\n");
}

late_initcall(rtk_crypto_test_init);
module_exit(rtk_crypto_test_exit);

MODULE_LICENSE("GPL");

