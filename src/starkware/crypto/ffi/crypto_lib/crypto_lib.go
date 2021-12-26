package crypto_lib

import "encoding/hex"
import (
	"fmt"
	"math/big"
)
/*

#cgo CFLAGS: -I.
#cgo LDFLAGS: -L./.. -lcrypto_c_exports -Wl,-rpath=./.
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
	padded := fmt.Sprintf("0x%064s", s[2:])
	return padded
}

/*
  Computes the StarkWare version of the Pedersen hash of x and y.
  Full specification of the hash function can be found here:
  https://docs.starkware.co/starkex-docs/crypto/pedersen-hash-function
*/
func Hash(input1, input2 string) string {
	input1_dec, _ := hex.DecodeString(reverseHexEndianRepresentation(input1))
	input2_dec, _ := hex.DecodeString(reverseHexEndianRepresentation(input2))
	in1 := C.CBytes(input1_dec)
	in2 := C.CBytes(input2_dec)
	var o [1024]byte
	out := C.CBytes(o[:])

	res := C.Hash(
		(*C.char)(unsafe.Pointer(in1)),
		(*C.char)(unsafe.Pointer(in2)),
		(*C.char)(unsafe.Pointer(out)))

	if res != 0 {
		fmt.Printf("Pedersen hash encountered an error: %s\n", C.GoBytes(unsafe.Pointer(out), 1024))
		C.free(unsafe.Pointer(in1))
		C.free(unsafe.Pointer(in2))
		C.free(unsafe.Pointer(out))
		return ""
	}

	hash_result := "0x" + reverseHexEndianRepresentation(
		hex.EncodeToString(C.GoBytes(unsafe.Pointer(out), 32)))

	C.free(unsafe.Pointer(in1))
	C.free(unsafe.Pointer(in2))
	C.free(unsafe.Pointer(out))

	return hash_result
}

/*
  Deduces the public key given a private key.
*/
func GetPublicKey(private_key string) string {
	private_key_dec, _ := hex.DecodeString(
		reverseHexEndianRepresentation(padHexString(private_key)))
	private_key_bytes := C.CBytes(private_key_dec)
	var o [1024]byte
	out := C.CBytes(o[:])

	res := C.GetPublicKey(
		(*C.char)(unsafe.Pointer(private_key_bytes)), (*C.char)(unsafe.Pointer(out)))

	if res != 0 {
		fmt.Printf("GetPublicKey encountered an error: %s\n", C.GoBytes(unsafe.Pointer(out), 1024))
		C.free(unsafe.Pointer(private_key_bytes))
		C.free(unsafe.Pointer(out))
		return ""
	}

	public_key_result := "0x" + reverseHexEndianRepresentation(
		hex.EncodeToString(C.GoBytes(unsafe.Pointer(out), 32)))

	C.free(unsafe.Pointer(private_key_bytes))
	C.free(unsafe.Pointer(out))

	return public_key_result
}

/*
  Verifies ECDSA signature of a given message hash z with a given public key.
  Returns true if public_key signs the message.
  NOTE: This function assumes that the public_key is on the curve.
*/
func Verify(stark_key, msg_hash, r, s string) bool {
	stark_key_dec, _ := hex.DecodeString(reverseHexEndianRepresentation(padHexString(stark_key)))
	stark_key_bytes := C.CBytes(stark_key_dec)

	message_dec, _ := hex.DecodeString(reverseHexEndianRepresentation(padHexString(msg_hash)))
	message_bytes := C.CBytes(message_dec)

	r_dec, _ := hex.DecodeString(reverseHexEndianRepresentation(padHexString(r)))
	r_bytes := C.CBytes(r_dec)

	w := invertOnCurve(s)
	w_dec, _ := hex.DecodeString(reverseHexEndianRepresentation(padHexString(w)))
	w_bytes := C.CBytes(w_dec)

	res := C.Verify(
		(*C.char)(unsafe.Pointer(stark_key_bytes)),
		(*C.char)(unsafe.Pointer(message_bytes)),
		(*C.char)(unsafe.Pointer(r_bytes)),
		(*C.char)(unsafe.Pointer(w_bytes)))

	C.free(unsafe.Pointer(stark_key_bytes))
	C.free(unsafe.Pointer(message_bytes))
	C.free(unsafe.Pointer(r_bytes))
	C.free(unsafe.Pointer(w_bytes))

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
	private_key_dec, _ := hex.DecodeString(
		reverseHexEndianRepresentation(padHexString(private_key)))
	private_key_bytes := C.CBytes(private_key_dec)

	message_dec, _ := hex.DecodeString(reverseHexEndianRepresentation(padHexString(message)))
	message_bytes := C.CBytes(message_dec)

	k_dec, _ := hex.DecodeString(reverseHexEndianRepresentation(padHexString(k)))
	k_bytes := C.CBytes(k_dec)

	var o [1024]byte
	out := C.CBytes(o[:])

	C.Sign(
		(*C.char)(unsafe.Pointer(private_key_bytes)),
		(*C.char)(unsafe.Pointer(message_bytes)),
		(*C.char)(unsafe.Pointer(k_bytes)),
		(*C.char)(unsafe.Pointer(out)))

	res := reverseHexEndianRepresentation(hex.EncodeToString(C.GoBytes(unsafe.Pointer(out), 64)))
	signature_w := "0x" + res[0:64]
	signature_r := "0x" + res[64:]

	signature_s := invertOnCurve(signature_w)

	C.free(unsafe.Pointer(private_key_bytes))
	C.free(unsafe.Pointer(message_bytes))
	C.free(unsafe.Pointer(k_bytes))
	C.free(unsafe.Pointer(out))

	return signature_r, signature_s
}
