const Web3 = require('web3');

// Infura 프로젝트 ID와 모니터링할 주소
const INFURA_PROJECT_ID = 'YOUR_INFURA_PROJECT_ID';
const MONITORED_ADDRESS = '0xYourAddressHere'.toLowerCase();

const web3 = new Web3(`https://mainnet.infura.io/v3/${INFURA_PROJECT_ID}`);

(async () => {
  // Pending 트랜잭션 구독 시작
  const subscription = web3.eth.subscribe('pendingTransactions', async (error, txHash) => {
    if (error) {
      console.error('Error subscribing to pending transactions:', error);
      return;
    }

    try {
      // 트랜잭션 세부정보 가져오기
      const tx = await web3.eth.getTransaction(txHash);

      // 유효한 트랜잭션인지 확인 후 모니터링 주소와 비교
      if (tx && tx.to && tx.to.toLowerCase() === MONITORED_ADDRESS) {
        console.log('Incoming transaction detected:', {
          from: tx.from,
          to: tx.to,
          value: web3.utils.fromWei(tx.value, 'ether'), // 값(wei)을 Ether로 변환
          gasPrice: web3.utils.fromWei(tx.gasPrice, 'gwei') + ' Gwei', // 가스 가격 변환
          hash: tx.hash,
        });
      }
    } catch (err) {
      console.error('Error fetching transaction details:', err.message);
    }
  });

  // 종료 시 구독 해제
  process.on('SIGINT', () => {
    subscription.unsubscribe((err, success) => {
      if (success) {
        console.log('Successfully unsubscribed from pending transactions.');
      } else {
        console.error('Error unsubscribing:', err);
      }
      process.exit();
    });
  });
})();
