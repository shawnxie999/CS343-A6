// CS343 - Concurrent and Parallel Programming
// Winter, 2014
// Assignment 6
#include "student.h"

const unsigned int MIN_PURCHASES = 1;
const unsigned int INITIAL_VALUE = 5;

Student::Student( Printer &prt, NameServer &nameServer, WATCardOffice &cardOffice, unsigned int id,
        unsigned int maxPurchases ):
    printer(prt),
    nameserver(nameserver),
    office(cardOffice),
    id(id),
    maxPurchases(maxPurchases){
    }

/* main function of student, student try to buy
 * some soda of his favorite flavour
 */
Student::main(){
    printer.print(Printer::Student, (char) STARTING);

    //generate purchase and flavour
    unsigned int purchase = mprng(MIN_PURCHASES, maxPurchases);
    unsigned int flavour = mprng(VendingMachine::NUM_FLAVOURS - 1);

    //create watcard
    FWATCard watcard = office.create(id, INITIAL_VALUE);

    //obtain location of machines
    VendingMachine *machine = nameserver.getMachine(id);
    printer.print(Printer::Student, id, (char) SELECT_MACHINE, machine->getId());

    //purchase
    unsigned int numPurchased = 0;
    while(numPurchased < purchase){
        yield(mprng(1,10));
        while(true){
            try{
                VendingMachine::Status st = machine->buy((VendingMachine::Flavours)flavour, *(watcard()));
                break;
            } catch(WATCardOffice::Lost){//card is lost
                printer.print(Printer::Student, id, (char)WATCARD_LOST);
                watcard = office.create(id, STARTING_WATCARD_AMOUNT);
            }
        }
        switch (st) {
            case VendingMachine::STOCK:
                //select another machine
                machine = nameserver.getMachine(id);
                printer.print(Printer::Student, id, (char) SELECT_MACHINE, machine->getId());
                break;
            case VendingMachine::FUNDS:
                //transfer money to watcard
                watcard = office.transfer(id, STARTING_WATCARD_AMOUNT + machine->cost(), watcard());
                break;
            case VendingMachine::BUY:
                //success
                numPurchased++;
                printer.print(Printer::Student, id, (char)VendingMachine::BUY, machine->getId());
                break;

        }
    }

    delete watcard();
    printer.print(Printer::Student, id, (char)FINISHED);

}