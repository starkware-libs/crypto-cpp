package crypto_lib

import "encoding/hex"
import (
	"fmt"
	"math/big"
	"errors"
)
/*

#cgo CFLAGS: -I.
#cgo LDFLAGS: -lcrypto_c_exports
#include <stdlib.h>
#include "../ecdsa.h"
#include "../pedersen_hash.h"

*/
import "C"
import "unsafe"


var curveOrder, _ = new(big.Int).SetString("800000000000010ffffffffffffffffb781126dcae7b2321e66a241adc64d2f", 16)

/*
  Computes the component s of a signature given the component w.
*/
func invertOnCurve(w string) string {
	w_big_int := new(big.Int)
	w_big_int.SetString(w[2:], 16)
	w_big_int.ModInverse(w_big_int, curveOrder)
	s := fmt.Sprintf("0x0%x", w_big_int)
	return s
}

func invertOnCurveBinary(w []byte) []byte {
	w_big_int := new(big.Int).SetBytes(w)
	w_big_int.ModInverse(w_big_int, curveOrder)
	return w_big_int.Bytes()
}

/*
  Given a hex string reverses its endian represenation.
*/
func reverseHexEndianRepresentation(s string) string {
	rns := []rune(s)
	for i, j := 0, len(rns)-2; i < j; i, j = i+2, j-2 {
		rns[i], rns[j] = rns[j], rns[i]
        rns[i+1], rns[j+1] = rns[j+1], rns[i+1]
    }
	return string(rns)
}

/*
  Pads the given hex string with leading zeroes so that its length is 64 characters (32 bytes).
*/
func padHexString(s string) string {
	padded := fmt.Sprintf("%064s", s[2:])
	return padded
}

/*
  Computes the StarkWare version of the Pedersen hash of x and y.
  Full specification of the hash function can be found here:
  https://docs.starkware.co/starkex-docs/crypto/pedersen-hash-function
*/
func Hash(input1, input2 string) string {
	input1_dec, _ := hex.DecodeString(padHexString(input1))
	input2_dec, _ := hex.DecodeString(padHexString(input2))
	var out [1024]byte

	if err := HashBinary(input1_dec, input2_dec, out[:]); err != nil {
		return ""
	}

	return "0x" + hex.EncodeToString(out[:32])
}

func HashBinary(input1, input2, out []byte) error {
	res := C.Hash(
		(*C.char)(unsafe.Pointer(&input1[0])),
		(*C.char)(unsafe.Pointer(&input2[0])),
		(*C.char)(unsafe.Pointer(&out[0])))

	if res != 0 {
		return errors.New("Pedersen hash encountered an error")
	}

	return nil
}

/*
  Deduces the public key given a private key.
*/
func GetPublicKey(private_key string) string {
	private_key_dec, _ := hex.DecodeString(padHexString(private_key))
	var out [1024]byte

	if err := GetPublicKeyBinary(private_key_dec, out[:]); err != nil {
		return ""
	}

	return "0x" + hex.EncodeToString(out[:32])
}

func GetPublicKeyBinary(private_key, out []byte) error {
	res := C.GetPublicKey(
		(*C.char)(unsafe.Pointer(&private_key[0])),
		(*C.char)(unsafe.Pointer(&out[0])))

	if res != 0 {
		return errors.New("GetPublicKey encountered an error")
	}

	return nil
}

/*
  Verifies ECDSA signature of a given message hash z with a given public key.
  Returns true if public_key signs the message.
  NOTE: This function assumes that the public_key is on the curve.
*/
func Verify(stark_key, msg_hash, r, s string) bool {
	stark_key_dec, _ := hex.DecodeString(padHexString(stark_key))
	message_dec, _ := hex.DecodeString(padHexString(msg_hash))
	r_dec, _ := hex.DecodeString(padHexString(r))
	s_dec, _ := hex.DecodeString(padHexString(s))

	return VerifyBinary(stark_key_dec, message_dec, r_dec, s_dec)
}

func VerifyBinary(stark_key, msg_hash, r, s []byte) bool {
	w := invertOnCurveBinary(s)

	res := C.Verify(
		(*C.char)(unsafe.Pointer(&stark_key[0])),
		(*C.char)(unsafe.Pointer(&msg_hash[0])),
		(*C.char)(unsafe.Pointer(&r[0])),
		(*C.char)(unsafe.Pointer(&w[0])))
	return res != 0;
}

/*
  Signs the given message hash with the provided private_key, with randomness k.

  NOTE: k should be a strong cryptographical random, and not repeat.
  See: https://tools.ietf.org/html/rfc6979.
*/
func Sign(private_key, message, k string) (string, string) {
	private_key_dec, _ := hex.DecodeString(padHexString(private_key))
	message_dec, _ := hex.DecodeString(padHexString(message))
	k_dec, _ := hex.DecodeString(padHexString(k))
	var out [1024]byte

	if err := SignBinary(private_key_dec,message_dec, k_dec, out[:]); err != nil {
		return "", ""
	}

	res := hex.EncodeToString(out[:64])
	signature_r := "0x" + res[0:64]
	signature_s := "0x" + res[64:]
	return signature_r, signature_s
}

func SignBinary(private_key, message, k, out []byte) error {
	ret := C.Sign(
		(*C.char)(unsafe.Pointer(&private_key[0])),
		(*C.char)(unsafe.Pointer(&message[0])),
		(*C.char)(unsafe.Pointer(&k[0])),
		(*C.char)(unsafe.Pointer(&out[0])))

	copy(out[32:64], invertOnCurveBinary(out[32:64]))

	if (ret != 0) {
		return errors.New("SignBinary encountered an error")
	}

	return nil
}