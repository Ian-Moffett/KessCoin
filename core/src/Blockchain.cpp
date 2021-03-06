#include "../include/Blockchain.hpp"


Blockchain::Blockchain() {
	this->current_node = new BlockNode;
	this->head_node = this->current_node;
	this->temp_node = this->current_node;
	this->current_node->next = NULL;
	this->tempStorageActive = false;

	std::ifstream blockno;
	blockno.open("../info/blockinfo/blockno");

	uint32_t blknoint;
	std::string blknostr;
	std::getline(blockno, blknostr);

	std::stringstream ss;
	ss << blknostr;
	ss >> blknoint;

	blockno.close();

	this->height = blknoint;
	this->lastHash = "";

	// TODO: Make blocks & transactions get restored if crash occurs (unlikely)
	Block* genesis = new Block({}, "0000000000000000000000000000000000000000000000000000000000000000", 0);

	this->lastHash = genesis->gethash();
	this->add_block(genesis);

}


Blockchain::~Blockchain() {
	delete this->genesis;
	this->genesis = NULL;

	BlockNode* curNode = this->head_node;
	BlockNode* nextNode;

	while (curNode != NULL) {
		nextNode = curNode->next;
		delete curNode;
		curNode = nextNode;
	}
}


void Blockchain::add_block(Block* blk) {
	this->current_node = new BlockNode;
	this->current_node->block = blk;
	this->current_node->prev = this -> temp_node;
	this->temp_node->next = this -> current_node;
	this->temp_node = this->current_node;
	this->current_node->next = NULL;
	std::ifstream blocknum;
	blocknum.open("../info/blockinfo/blockno");
	std::string blockno = "";
	std::getline(blocknum, blockno);
	std::stringstream ss;
	ss << blockno;
	uint64_t blockindex;
	ss >> blockindex;
	++blockindex;

	std::ofstream outfile;
	outfile.open("../info/blockinfo/blockno");
	outfile << std::to_string(blockindex);

	outfile.close();
	blocknum.close();
}


void Blockchain::add_pending_transactions(std::vector<Transaction> t) {
	this->pending_transactions = t;
}


void Blockchain::delete_temp_storage() {
	if (tempStorageActive) {
		std::cout << "\n\nPlease be careful abnormally halting process." << std::endl;
		delete this->tempStorage;
	}
}


void Blockchain::increment_height() {
	std::ofstream of;
	of.open("../info/blockinfo/blockno");
	of << ++this->height;
	of << std::to_string(this->height);  // Updates the height.
	of.close();
}


void Blockchain::mine_pending_transactions() {
	this->tempStorageActive = true;
	Block* newBlock = new Block(this->pending_transactions, this->lastHash, this->height);
	this->tempStorage = newBlock;

	newBlock->mine(4);  // 2 for now.
	this->add_block(newBlock);

	std::ofstream blockout;
	char blockpath[250];

	sprintf(blockpath, "../info/blockinfo/blocks/block-%d", this->height);
	blockout.open(blockpath, std::ios_base::app);

	blockout << newBlock->gethash();
	blockout << newBlock->get_prev_hash();
	blockout << newBlock->get_timestamp();
	blockout.close(); 

	std::cout << "\n********** BLOCK ADDED TO BLOCKCHAIN **********" << std::endl;
	std::cout << "Hash: " << newBlock->gethash() << std::endl;
	std::cout << "Previous Hash: " << newBlock->get_prev_hash() << std::endl;
	std::cout << "Timestamp: " << newBlock->get_timestamp() << std::endl;
	std::cout << "***********************************************" << std::endl;
	this->lastHash = newBlock->gethash();
}
