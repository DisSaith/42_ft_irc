#ifndef OPERATOR_HPP
# define OPERATOR_HPP

#ifndef DEBUG
# define DEBUG 1
#endif

# include "Client.hpp"

class Operator: public Client
{
	public:
		/* ----- Orthodox Canonical Form ----- */
		~Operator();
		Operator();
		Operator(int fd);
		Operator(const Operator& copy);
		Operator& operator=(const Operator& copy);

		/* ----- Methods ----- */
};

#endif
