const swCrypto = require('../crypto.js');
const chai = require('chai');
const { expect } = chai;

describe('Pedersen Hash', () => {
    it('should hash correctly', () => {
        const testData = require('./signature_test_data.json');
        for (const hashTestData of [
            testData.hash_test.pedersen_hash_data_1, testData.hash_test.pedersen_hash_data_2
        ]) {
            expect(
                swCrypto.pedersen(
                    BigInt(hashTestData.input_1),
                    BigInt(hashTestData.input_2)
                )
            ).to.equal(
                BigInt(hashTestData.output)
            );
        }
    });
});

describe('getPublicKey', () => {
    it('should derive correct public key', () => {
        expect(
            swCrypto.getPublicKey(BigInt('0x1'))
        ).to.equal(
            BigInt('0x1ef15c18599971b7beced415a40f0c7deacfd9b0d1819e03d723d8bc943cfca')
        );
    });
});

describe('Verify', () => {
    const stark_key = BigInt('0x1ef15c18599971b7beced415a40f0c7deacfd9b0d1819e03d723d8bc943cfca');
    const message = BigInt('0x2');
    const r = BigInt('0x411494b501a98abd8262b0da1351e17899a0c4ef23dd2f96fec5ba847310b20');
    const s = BigInt('0x405c3191ab3883ef2b763af35bc5f5d15b3b4e99461d70e84c654a351a7c81b');
    it('should verify', () => {
        expect(swCrypto.verify(stark_key, message, r, s)).to.equal(true);
    });
});

describe('Sign', () => {
    const private_key = BigInt('0x1');
    const message = BigInt('0x2');
    const k = BigInt('0x3');
    it('correct signature', () => {
        expected_r = BigInt('0x411494b501a98abd8262b0da1351e17899a0c4ef23dd2f96fec5ba847310b20')
        expected_s = BigInt('0x405c3191ab3883ef2b763af35bc5f5d15b3b4e99461d70e84c654a351a7c81b')
        const { r, s } = swCrypto.sign(private_key, message, k)
        expect(r).to.equal(expected_r);
        expect(s).to.equal(expected_s);
    });
});
