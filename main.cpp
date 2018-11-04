#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <fstream>
#include <thread>

using boost::asio::ip::tcp;

#define server_port 2000

//поток сервера
void server(boost::asio::io_service* io_service, std::string filename) {
  try
  {
    tcp::acceptor acceptor(*io_service, tcp::endpoint(tcp::v4(), server_port));

    while(true)
    {
      tcp::socket socket(*io_service);
      acceptor.accept(socket);

      std::ifstream fin(filename, std::ifstream::binary);
      if(!fin.is_open()) {
        std::cout << "Не могу открыть файл: " << filename << std::endl;

        exit(-1);
      }

      std::vector<char> buffer (1024,0);

      while(!fin.eof()) {
          fin.read(buffer.data(), buffer.size());
          std::streamsize read = fin.gcount();
          boost::asio::write(socket, boost::asio::buffer(buffer.data(), read),
	          boost::asio::transfer_all());
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

int main(int argc, const char* argv[]) {
  if(argc != 2) {
    std::cout << "Передайте имя входного файла" << std::endl;
    return -1;
  }

  std::string input_filename = argv[1];

  boost::asio::io_service io_service;
  std::thread t(std::bind(&server, &io_service, input_filename));
  t.detach();

  try
  {
    boost::asio::io_service io_service;

    tcp::socket socket(io_service);
    socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), server_port));

    std::string o_file_name;
    std::cout << "Введите имя выходного файла" << std::endl;
    std::cin >> o_file_name;

    std::ofstream of(o_file_name);


    std::array<char, 1024> buf;
    boost::system::error_code error;

    while(true) {
      size_t len = socket.read_some(boost::asio::buffer(buf), error);

      if (error == boost::asio::error::eof) {
        std::cout << "Файл скачан!" << std::endl;
        return 0;
      }
      else if (error)
        throw boost::system::system_error(error);
	of.write(buf.data(), len);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return -1;
}
