// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "THORSwap/Swap.h"
#include "HexCoding.h"
#include "Coin.h"
#include "proto/Binance.pb.h"
#include <TrustWalletCore/TWCoinType.h>
#include <TrustWalletCore/TWAnySigner.h>
#include "../interface/TWTestUtilities.h"

#include <gtest/gtest.h>

namespace TW::THORSwap {

// Addresses for wallet 'isolate dismiss fury ... note'
const auto Address1Btc = "bc1qpjult34k9spjfym8hss2jrwjgf0xjf40ze0pp8";
const auto Address1Eth = "0xB9F5771C27664bF2282D98E09D7F50cEc7cB01a7";
const auto Address1Bnb = "bnb1us47wdhfx08ch97zdueh3x3u5murfrx30jecrx";
const auto Address1Thor = "thor1z53wwe7md6cewz9sqwqzn0aavpaun0gw0exn2r";
const Data PrivateKey1Btc = parse_hex("13fcaabaf9e71ffaf915e242ec58a743d55f102cf836968e5bd4881135e0c52c");
const Data PrivateKey1Bnb = parse_hex("bcf8b072560dda05122c99390def2c385ec400e1a93df0657a85cf6b57a715da");
const auto VaultEth = "0x5d5fa69cace0352bf520377e055a34a9f8f7257c";
const auto VaultBnb = "bnb1n9esxuw8ca7ts8l6w66kdh800s09msvul6vlse";


TEST(THORSwap, SwapBtcEth) {
    auto res = Swap::build(Chain::BTC, Chain::ETH, Address1Btc, "ETH", "", Address1Eth, VaultEth, "10000000", "10000000");
    EXPECT_EQ(res.second, "Invalid from chain: 2");
}

TEST(THORSwap, SwapBnbBtc) {
    auto res = Swap::build(Chain::BNB, Chain::BTC, Address1Bnb, "BTC", "", Address1Btc, VaultBnb, "10000000", "10000000");
    ASSERT_EQ(res.second, "");
    EXPECT_EQ(hex(res.first), "0a1242696e616e63652d436861696e2d4e696c652a40535741503a4254432e4254433a62633171706a756c7433346b3973706a66796d38687373326a72776a676630786a6634307a65307070383a313030303030303052480a220a14e42be736e933cf8b97c26f33789a3ca6f8348cd1120a0a03424e421080ade20412220a1499730371c7c77cb81ffa76b566dcef7c1e5dc19c120a0a03424e421080ade204");

    auto tx = Binance::Proto::SigningInput();
    ASSERT_TRUE(tx.ParseFromArray(res.first.data(), (int)res.first.size()));

    // check fields
    auto pk2 = tx.private_key();
    EXPECT_EQ(hex(TW::data(pk2)), "");
    EXPECT_EQ(tx.memo(), "SWAP:BTC.BTC:bc1qpjult34k9spjfym8hss2jrwjgf0xjf40ze0pp8:10000000");
    ASSERT_EQ(tx.send_order().inputs_size(), 1);
    ASSERT_EQ(tx.send_order().outputs_size(), 1);
    EXPECT_EQ(hex(tx.send_order().inputs(0).address()), "e42be736e933cf8b97c26f33789a3ca6f8348cd1");
    EXPECT_EQ(hex(tx.send_order().outputs(0).address()), "99730371c7c77cb81ffa76b566dcef7c1e5dc19c");

    // sign and encode resulting input
    tx.set_private_key(PrivateKey1Bnb.data(), PrivateKey1Bnb.size());
    Binance::Proto::SigningOutput output;
    ANY_SIGN(tx, TWCoinTypeBinance);
    EXPECT_EQ(hex(output.encoded()), "8002f0625dee0a4c2a2c87fa0a220a14e42be736e933cf8b97c26f33789a3ca6f8348cd1120a0a03424e421080ade20412220a1499730371c7c77cb81ffa76b566dcef7c1e5dc19c120a0a03424e421080ade204126a0a26eb5ae9872103ea4b4bc12dc6f36a28d2c9775e01eef44def32cc70fb54f0e4177b659dbc0e1912404836ee8659caa86771281d3f104424d95977bdedf644ec8585f1674796fde525669a6d446f72da89ee90fb0e064473b0a2159a79630e081592c52948d03d67071a40535741503a4254432e4254433a62633171706a756c7433346b3973706a66796d38687373326a72776a676630786a6634307a65307070383a3130303030303030");
}

TEST(THORSwap, SwapBnbRune) {
    auto res = Swap::build(Chain::BNB, Chain::THOR, Address1Bnb, "RUNE", "", Address1Thor, VaultBnb, "4000000", "121065076");
    ASSERT_EQ(res.second, "");
    EXPECT_EQ(hex(res.first), "0a1242696e616e63652d436861696e2d4e696c652a44535741503a54484f522e52554e453a74686f72317a3533777765376d64366365777a39737177717a6e306161767061756e3067773065786e32723a31323130363530373652480a220a14e42be736e933cf8b97c26f33789a3ca6f8348cd1120a0a03424e42108092f40112220a1499730371c7c77cb81ffa76b566dcef7c1e5dc19c120a0a03424e42108092f401");

    auto tx = Binance::Proto::SigningInput();
    ASSERT_TRUE(tx.ParseFromArray(res.first.data(), (int)res.first.size()));

    // check fields
    auto pk2 = tx.private_key();
    EXPECT_EQ(hex(TW::data(pk2)), "");
    EXPECT_EQ(tx.memo(), "SWAP:THOR.RUNE:thor1z53wwe7md6cewz9sqwqzn0aavpaun0gw0exn2r:121065076");
    ASSERT_EQ(tx.send_order().inputs_size(), 1);
    ASSERT_EQ(tx.send_order().outputs_size(), 1);
    EXPECT_EQ(hex(tx.send_order().inputs(0).address()), "e42be736e933cf8b97c26f33789a3ca6f8348cd1");
    EXPECT_EQ(hex(tx.send_order().outputs(0).address()), "99730371c7c77cb81ffa76b566dcef7c1e5dc19c");

    // set private key and few other fields
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeBinance, PrivateKey(PrivateKey1Bnb)), Address1Bnb);
    tx.set_private_key(PrivateKey1Bnb.data(), PrivateKey1Bnb.size());
    tx.set_chain_id("Binance-Chain-Tigris");
    tx.set_account_number(1902570);
    tx.set_sequence(4);
    // sign and encode resulting input
    Binance::Proto::SigningOutput output;
    ANY_SIGN(tx, TWCoinTypeBinance);
    EXPECT_EQ(hex(output.encoded()), "8a02f0625dee0a4c2a2c87fa0a220a14e42be736e933cf8b97c26f33789a3ca6f8348cd1120a0a03424e42108092f40112220a1499730371c7c77cb81ffa76b566dcef7c1e5dc19c120a0a03424e42108092f40112700a26eb5ae9872103ea4b4bc12dc6f36a28d2c9775e01eef44def32cc70fb54f0e4177b659dbc0e191240d91b6655ea4ade62a90cc9b28e43ccd2887dcf1c563e42bbd0d6ae4e825c2c6a1ba7784866810f36b6e098b0c877d1daa48016d0558f7b796b3f0b410107ba2f18ea8f7420041a44535741503a54484f522e52554e453a74686f72317a3533777765376d64366365777a39737177717a6e306161767061756e3067773065786e32723a313231303635303736");

    // see https://explorer.binance.org/tx/84EE429B35945F0568097527A084532A9DE7BBAB0E6A5562E511CEEFB188DE69
    // https://viewblock.io/thorchain/tx/D582E1473FE229F02F162055833C64F49FB4FF515989A4785ED7898560A448FC
}

TEST(THORSwap, Memo) {
    EXPECT_EQ(Swap::buildMemo(Chain::BTC, "BNB", "bnb123", 1234), "SWAP:BTC.BNB:bnb123:1234");
}

TEST(THORSwap, WrongFromAddress) {
    {
        auto res = Swap::build(Chain::BNB, Chain::ETH, "DummyAddress", "ETH", "", Address1Eth, VaultEth, "100000", "100000");
        EXPECT_EQ(res.second, "Invalid from address");
    }
    {
        auto res = Swap::build(Chain::BNB, Chain::ETH, Address1Btc, "ETH", "", Address1Eth, VaultEth, "100000", "100000");
        EXPECT_EQ(res.second, "Invalid from address");
    }
}

TEST(THORSwap, WrongToAddress) {
    {
        auto res = Swap::build(Chain::BNB, Chain::ETH, Address1Bnb, "ETH", "", "DummyAddress", VaultEth, "100000", "100000");
        EXPECT_EQ(res.second, "Invalid to address");
    }
    {
        auto res = Swap::build(Chain::BNB, Chain::ETH, Address1Bnb, "ETH", "", Address1Btc, VaultEth, "100000", "100000");
        EXPECT_EQ(res.second, "Invalid to address");
    }
}

} // namespace