#include <iostream>
#include "Article.h"
#include "ArticleDatabase.h"
#include "Message.h"
#include "Peer.h"
#include "Networking.h"

int main() {
	ArticleHeaderMessage m(123, 255, 123, 124);
	auto s = m.Serialize();
	ArticleHeaderMessage n(0,0,0,0);
	n.Deserialize(s);
	
	std::cout << "Hello World!!" << std::endl;
	return 0;
}
