package crypto_lib

import "encoding/hex"
import (
	"fmt"
	"math/big"
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


const curveOrder = "800000000000010ffffffffffffffffb781126dcae7b2321e66a241adc64d2f"

/*
  Computes the component s of a signature given the component w.
*/
func invertOnCurve(w string) string {
	w_big_int := new(big.Int)
	w_big_int.SetString(w[2:], 16)
	order := new(big.Int)
	order.SetString(curveOrder, 16)
	w_big_int.ModInverse(w_big_int, order)
	s := fmt.Sprintf("0x0%x", w_big_int)
	return s
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

	res := C.Hash(
		(*C.char)(unsafe.Pointer(&input1_dec[0])),
		(*C.char)(unsafe.Pointer(&input2_dec[0])),
		(*C.char)(unsafe.Pointer(&out[0])))

	if res != 0 {
		fmt.Printf("Pedersen hash encountered an error: %s\n", out)
		return ""
	}
	return "0x" + hex.EncodeToString(out[:32])
}

/*
  Deduces the public key given a private key.
*/
func GetPublicKey(private_key string) string {
	private_key_dec, _ := hex.DecodeString(padHexString(private_key))
	var out [1024]byte

	res := C.GetPublicKey((*C.char)(unsafe.Pointer(&private_key_dec[0])),
		(*C.char)(unsafe.Pointer(&out[0])))

	if res != 0 {
		fmt.Printf("GetPublicKey encountered an error: %s\n", C.GoBytes(unsafe.Pointer(&out[0]), 1024))
		return ""
	}

	return "0x" + hex.EncodeToString(out[:32])
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

	w := invertOnCurve(s)
	w_dec, _ := hex.DecodeString(padHexString(w))

	res := C.Verify(
		(*C.char)(unsafe.Pointer(&stark_key_dec[0])),
		(*C.char)(unsafe.Pointer(&message_dec[0])),
		(*C.char)(unsafe.Pointer(&r_dec[0])),
		(*C.char)(unsafe.Pointer(&w_dec[0])))

	if res == 0 {
		return false
	}
	return true
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

	ret := C.Sign(
		(*C.char)(unsafe.Pointer(&private_key_dec[0])),
		(*C.char)(unsafe.Pointer(&message_dec[0])),
		(*C.char)(unsafe.Pointer(&k_dec[0])),
		(*C.char)(unsafe.Pointer(&out[0])))

	if (ret != 0) {
		return "", ""
	}

	res := hex.EncodeToString(out[:64])
	signature_r := "0x" + res[0:64]
	signature_w := "0x" + res[64:]

	signature_s := invertOnCurve(signature_w)

	return signature_r, signature_s
}
