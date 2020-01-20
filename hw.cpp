/* 
MIT License 

Copyright (c) 2019 МГТУ им. Н.Э. Баумана, кафедра ИУ-6, Михаил Фетисов, 

Программа-заготовка для домашнего задания
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>

#include <sstream>

#include "collector.h"

using namespace std;

const size_t MAX_NAME_LENGTH    = 50;
const size_t MIN_CODE  = 0;
const size_t MAX_CODE  = 9999;

class EcoCard
{
protected:
    bool invariant() const
    {
        return _code_okato >= MIN_CODE
            && _code_okato <= MAX_CODE
            && !_community_name.empty()
            && _community_name.size() <= MAX_NAME_LENGTH
            && !_level_of_pollution.empty()
            && _level_of_pollution.size() <= MAX_NAME_LENGTH
            && _number_of_people >= 1;
    }

public:
    EcoCard() = delete;

    EcoCard(const string & community_name, int code_okato, int number_of_people, const string &  level_of_pollution)
            : _community_name(community_name), _code_okato(code_okato), _number_of_people(number_of_people), _level_of_pollution(level_of_pollution)
    {
        assert(invariant());
    }

    const string & getCommName() const { return _community_name; }
    int       getCode() const { return _code_okato; }
    int       getNumberofPpl() const { return _number_of_people; }
    const string & getLevel() const{return _level_of_pollution;}

    static bool    write(const EcoCard &p, ostream& os)
    {
        writeString(os, p.getCommName());
        writeNumber(os, p.getCode());
        writeNumber(os, p.getNumberofPpl());
        writeString(os, p.getLevel());

        return os.good();
    }

    static EcoCard  read(istream& is)
    {
        string   _community_name = readString(is, MAX_NAME_LENGTH);
        int   _code_okato = readNumber<int>(is);
        int _number_of_people = readNumber<int>(is);
        string _level_of_pollution = readString(is, MAX_NAME_LENGTH);

        return EcoCard(_community_name, _code_okato, _number_of_people, _level_of_pollution);
    }

private:
    string   _community_name;
    int   _code_okato;
    int _number_of_people;
    string _level_of_pollution;
};

class Creator {
public:
    /* Fabric method*/
    virtual EcoCard createCard(string commName, int code, int NumOfPpl, string PolLvl) = 0;
};

class SuperCreator : Creator {
public:
    EcoCard createCard(string commName, int code, int NumOfPpl, string PolLvl) override {
        return EcoCard(commName, code, NumOfPpl, PolLvl);
    }
};
bool performCommand(const vector<string> & args, Collector<EcoCard> & col)
{
    SuperCreator creator;

    if (args.empty())
        return false;

    if (args[0] == "l" || args[0] == "load")
    {
        string filename = (args.size() == 1) ? "hw.data" : args[1];

        if (!col.loadCollection(filename))
        {
            cerr << "Ошибка при загрузке файла '" << filename << "'" << endl;
            return false;
        }

        return true;
    }

    if (args[0] == "s" || args[0] == "save")
    {
        string filename = (args.size() == 1) ? "hw.data" : args[1];

        if (!col.saveCollection(filename))
        {
            cerr << "Ошибка при сохранении файла '" << filename << "'" << endl;
            return false;
        }

        return true;
    }

    if (args[0] == "c" || args[0] == "clean")
    {
        if (args.size() != 1)
        {
            cerr << "Некорректное количество аргументов команды clean" << endl;
            return false;
        }

        col.clean();

        return true;
    }

    if (args[0] == "a" || args[0] == "add")
    {
        if (args.size() != 5)
        {
            cerr << "Некорректное количество аргументов команды add" << endl;
            return false;
        }

        col.addItem(creator.createCard(args[1].c_str(), stoul(args[2]), stoul(args[3]), args[4].c_str()));
        return true;
    }

    if (args[0] == "u" || args[0] == "update")
    {
        if (args.size() != 6)
        {
            cerr << "Некорректное количество аргументов команды update" << endl;
            return false;
        }

        col.updateItem(stoul(args[1]), EcoCard(args[1].c_str(), stoul(args[2]), stoul(args[3]), args[4].c_str()));
        return true;
    }

    if (args[0] == "r" || args[0] == "remove")
    {
        if (args.size() != 2)
        {
            cerr << "Некорректное количество аргументов команды remove" << endl;
            return false;
        }

        col.removeItem(stoul(args[1]));
        return true;
    }

    if (args[0] == "v" || args[0] == "view")
    {
        if (args.size() != 1)
        {
            cerr << "Некорректное количество аргументов команды view" << endl;
            return false;
        }

        size_t count = 0;
        for(size_t i=0; i < col.getSize(); ++i)
        {
            EcoCard item = col.getItem(i);

            if (!col.isRemoved(i))
            {
                cout << "[" << i << "] " 
                        << item.getCommName() << " "
                        << item.getCode() << " "
                        << item.getNumberofPpl() << " "
                        << item.getLevel() << endl;
                count ++;
            }
        }

        cout << "Количество элементов в коллекции: " << count << endl;

        return true;
    }
    if (args[0] == "--help" || args[0] == "-h") {
        cout << "-h, --help - выводит доступные команды" << endl;
        cout << "-с, --card - выводит информацию о структуре карточки" << endl;
        cout << "l, load - загрузка коллекции из файла data.hw" << endl;
        cout << "s, save - сохранение коллекции в файл data.hw" << endl;
        cout << "c, clean - очистка текущей коллекции в буфере" << endl;
        cout << "a, add - добавление карточки в коллекцию" << endl;
        cout << "u, update - обновления карточки коллекции по индексу" << endl;
        cout << "r, remove - удаление карточки из коллекции по индексу" << endl;
        cout << "v, view - просмотр текущей коллекции в буфере" << endl;

        return true;
    }

    if (args[0] == "--card" || args[0] == "-c") {
        cout << "Эко карта: { название_пункта код_отако количество_людей уровень_загрязнения }" << endl;

        return true;
    }

    if (args[0] == "--exit" || args[0] == "-e") {
        cout << "Выполнение завершено успешно" << endl;

        return false;
    }


    cerr << "Недопустимая команда '" << args[0] << "'" << endl;
    return false;
}

int main(int , char **)
{
    Collector<EcoCard> col;

    for (string line; getline(cin, line); )
    {
        if (line.empty())
            break;

        istringstream  iss(line);
        vector<string> args;
    
        for(string str; iss.good();)
        {
            iss >> str;
            args.emplace_back(str);
        }

        if (!performCommand(args, col))
            return 1;
    }

    cout << "Выполнение завершено успешно" << endl;
    return 0;
}