//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//
// Licensed under MIT License.
//

import Foundation

//
// script to generate list of badge ids
//
// swift badges.swift > badges.txt
//

func isValidBadge(_ num: Int) -> Bool {
    let n1 = num & 0b111
    let n2 = (num>>3) & 0b111
    let n3 = (num>>6) & 0b111
    let n4 = (num>>9) & 0b111
    
    if (n1 == n2) ||
        (n1 == n3) ||
        (n1 == n4) ||
        (n2 == n3) ||
        (n2 == n4) ||
        (n3 == n4) ||
        (n1 == 0) || (n2 == 0) || (n3 == 0) || (n4 == 0)
    {
        return false
    }
    
    return true
}


func zeropad(_ s: String,_ len: Int) -> String {
    var res = s
    while (res.count<len) {
        res = "0"+res
    }
    return res
}

func tobin(_ num: Int) -> String {
    return zeropad(String(num, radix: 2),12)
}

func tohex(_ num: Int) -> String {
    return zeropad(String(num, radix: 16),2)
}

func toeeprom(_ num: Int) -> String {
    return "0x\(tohex(num & 0xff)),0x\(tohex((num>>8) & 0xff)),0x0"
}

func ton(_ num: Int) -> String {
    return zeropad(String(num, radix: 8),4)
}

func tolen(_ num: Int) -> String {
    return zeropad(String(num, radix: 10),4)
}

let results = (0..<4096).filter(isValidBadge(_:)).shuffled()
var cnt = 1
results.forEach {
    print("\(cnt)\t\(tolen($0))\t\(tobin($0))\t\(toeeprom($0))\t\(ton($0))")
    cnt += 1
}
