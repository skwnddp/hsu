require('dotenv').config(); // Load environment variables
const Web3 = require('web3');

// Securely load Infura project ID and Ethereum address from environment variables
const INFURA_PROJECT_ID = process.env.INFURA_PROJECT_ID;
const MONITORED_ADDRESS = process.env.MONITORED_ADDRESS.toLowerCase();

if (!INFURA_PROJECT_ID || !MONITORED_ADDRESS) {
  console.error('Please set INFURA_PROJECT_ID and MONITORED_ADDRESS in your environment variables.');
  process.exit(1);
}

const web3 = new Web3(`https://mainnet.infura.io/v3/${INFURA_PROJECT_ID}`);

(async () => {
  try {
    const subscription = web3.eth.subscribe('pendingTransactions', async (error, txHash) => {
      if (error) {
        console.error('Error subscribing to pending transactions:', error);
        return;
      }

      try {
        const tx = await web3.eth.getTransaction(txHash);

        // Validate transaction and check if it is sent to the monitored address
        if (tx && tx.to && tx.to.toLowerCase() === MONITORED_ADDRESS) {
          console.log('Incoming transaction detected:', {
            from: tx.from,
            to: tx.to,
            value: web3.utils.fromWei(tx.value, 'ether'), // Convert value to Ether for better readability
            gasPrice: web3.utils.fromWei(tx.gasPrice, 'gwei') + ' Gwei',
            hash: tx.hash,
          });
        }
      } catch (err) {
        console.error('Error fetching transaction details:', err.message);
      }
    });

    // Graceful shutdown on process exit
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
  } catch (mainError) {
    console.error('An error occurred:', mainError.message);
  }
})();
