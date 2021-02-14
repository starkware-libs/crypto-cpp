/////////////////////////////////////////////////////////////////////////////////
// Copyright 2019 StarkWare Industries Ltd.                                    //
//                                                                             //
// Licensed under the Apache License, Version 2.0 (the "License").             //
// You may not use this file except in compliance with the License.            //
// You may obtain a copy of the License at                                     //
//                                                                             //
// https://www.starkware.co/open-source-license/                               //
//                                                                             //
// Unless required by applicable law or agreed to in writing,                  //
// software distributed under the License is distributed on an "AS IS" BASIS,  //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    //
// See the License for the specific language governing permissions             //
// and limitations under the License.                                          //
/////////////////////////////////////////////////////////////////////////////////

const BN = require('bn.js');
const BigIntBuffer = require('bigint-buffer');
const { assert } = require('chai');
const ffi = require('ffi-napi');

// Native crypto bindings.
const libcrypto = ffi.Library('./libcrypto_c_exports', {
    'Hash': ['int', ['string', 'string', 'string']],
    'Verify': ['bool', ['string', 'string', 'string', 'string']],
    'Sign': ['int', ['string', 'string', 'string', 'string']],
    'GetPublicKey': ['int', ['string', 'string']],
});

const curveOrder = new BN('800000000000010ffffffffffffffffb781126dcae7b2321e66a241adc64d2f', 16)

/*
 Computes the StarkWare version of the Pedersen hash of x and y.
 Full specification of the hash function can be found here:
 https://docs.starkware.co/starkex-docs/crypto/pedersen-hash-function
*/
function pedersen(x, y) {
    const x_buf = BigIntBuffer.toBufferLE(x, 32);
    const y_buf = BigIntBuffer.toBufferLE(y, 32);
    const res_buf = Buffer.alloc(1024);
    const res = libcrypto.Hash(x_buf, y_buf, res_buf);
    assert(res == 0, 'Error: ' + res_buf.toString('utf-8'));
    return BigIntBuffer.toBigIntLE(res_buf);
}

/*
 Verifies ECDSA signature of a given message hash z with a given public key.
 Returns true if public_key signs the message.
 NOTE: This function assumes that the public_key is on the curve.
*/
function verify(stark_key, message_hash, r, s) {
    const stark_key_buf = BigIntBuffer.toBufferLE(stark_key, 32);
    const message_hash_buf = BigIntBuffer.toBufferLE(message_hash, 32);
    const r_buf = BigIntBuffer.toBufferLE(r, 32);
    const bnS = new BN(s.toString(16), 16);
    const w = BigInt('0x' + bnS.invm(curveOrder).toString(16), 16)
    const s_buf = BigIntBuffer.toBufferLE(w, 32);
    return libcrypto.Verify(stark_key_buf, message_hash_buf, r_buf, s_buf);
}

/*
 Signs the given message hash with the provided private_key, with randomness k.

 NOTE: k should be a strong cryptographical random, and not repeat.
 See: https://tools.ietf.org/html/rfc6979.
*/
function sign(private_key, message, k) {
    const private_key_buf = BigIntBuffer.toBufferLE(private_key, 32);
    const message_buf = BigIntBuffer.toBufferLE(message, 32);
    const k_buf = BigIntBuffer.toBufferLE(k, 32);
    const res_buf = Buffer.alloc(1024);
    const res = libcrypto.Sign(private_key_buf, message_buf, k_buf, res_buf);
    assert(res == 0, 'Error: ' + res_buf.toString('utf-8'));
    const r = BigIntBuffer.toBigIntLE(res_buf.slice(0, 32));
    const w = BigIntBuffer.toBigIntLE(res_buf.slice(32, 64));
    const bnW = new BN(w.toString(16), 16)
    const s = BigInt('0x' + bnW.invm(curveOrder).toString(16), 16)
    return {r: r, s: s};
}

/*
 Deduces the public key given a private key.
 The x coordinate of the public key is also known as the partial public key,
 and used in StarkEx to identify the user.
*/
function getPublicKey(private_key) {
    const private_key_buf = BigIntBuffer.toBufferLE(private_key, 32);
    const res_buf = Buffer.alloc(1024);
    const res = libcrypto.GetPublicKey(private_key_buf, res_buf);
    assert(res == 0, 'Error: ' + res_buf.toString('utf-8'));
    return BigIntBuffer.toBigIntLE(res_buf);
}

module.exports = {
    pedersen,
    sign,
    verify,
    getPublicKey
};
