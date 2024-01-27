// SPDX-License-Identifier: MIT
pragma solidity ^0.8.17;

import "./IERC20.sol";

contract ERC20 is IERC20 {
    //총 공급량 변수 선언 
    uint public totalSupply;
    //소유자별로 토큰 잔액을 내부 테이블로 구현한다. 
    //이러면 토큰 컨트랙트에서 토큰을 소유한 사람을 추적할 수 있다. 
    mapping(address => uint) public balanceOf;
    //허용량의 데이터 매핑이다. 
    // ERC20 토큰을 사용하면 토큰 소유자가 권한을 위임자에게 위임하여 소유자의 잔액에서 특정 금액을 지출할 수 있다.  
    // ERC20 컨트랙트는 기본 키가 토큰 소유자의 주소고 지출자 주소와 허용 한도에 매핑되는 2차원 매핑으로 허용량을 추적한다. 
    mapping(address => mapping(address => uint)) public allowance;
    // 사람이 읽을 수 있는 토큰 
    string public name = "Yongari";
    //사람이 읽을 수 있는 기호 
    string public symbol = "YongTT";
    //토큰 양을 나눌 수 있는 소수 자릿수 반환 
    uint8 public decimals = 18;

    //출금 함수 
    function transfer(address recipient, uint amount) external returns (bool) {
        balanceOf[msg.sender] -= amount;
        balanceOf[recipient] += amount;
        emit Transfer(msg.sender, recipient, amount);
        return true;
    }

    //approve 후 transferFrom을 사용하는 트랜잭션 워크플로우 
    //토큰 소유자가 제어를 다른 주소에 위임할 수 있게 해준다. 이것은 제어를 토큰 배포 컨트랙트에 위임하는데 
    //가장 많이 사용되지만 거래소에서도 많이 사용된다. 
    function approve(address spender, uint amount) external returns (bool) {
        allowance[msg.sender][spender] = amount;
        emit Approval(msg.sender, spender, amount);
        return true;
    }

    //approve 후 transferFrom을 사용하는 트랜잭션 워크플로우 
    function transferFrom(
        address sender,
        address recipient,
        uint amount
    ) external returns (bool) {
        allowance[sender][msg.sender] -= amount;
        balanceOf[sender] -= amount;
        balanceOf[recipient] += amount;
        emit Transfer(sender, recipient, amount);
        return true;
    }

    //새 토큰을 만드는 함수 
    //ERC20 표준함수는 아니지만 많은 경우에 볼 수 있음
    function mint(uint amount) external {
        balanceOf[msg.sender] += amount;
        totalSupply += amount;
        emit Transfer(address(0), msg.sender, amount);
    }

    //토큰을 소각하는 함수 
    //ERC20 표준함수는 아니지만 많은 경우에 볼 수 있음
    function burn(uint amount) external {
        balanceOf[msg.sender] -= amount;
        totalSupply -= amount;
        emit Transfer(msg.sender, address(0), amount);
    }
}
