#include "blockchain.h"
#include "utils.h"

typedef neb::nebulas::nebulas_transaction_db nebulas_transaction_db_t;
typedef std::shared_ptr<nebulas_transaction_db_t> nebulas_transaction_db_ptr_t;
typedef neb::account_db<neb::nebulas_db> nebulas_account_db_t;
typedef std::shared_ptr<nebulas_account_db_t> nebulas_account_db_ptr_t;

void aql_query_result_traverse(const nebulas_transaction_db_ptr_t ptr) {
  auto result = ptr->aql_query("for item in transaction return item");
  auto documents = result->slices().front().get("result");
  for (size_t i = 0; i < documents.length(); i++) {
    auto doc = documents.at(i);
    LOG(INFO) << doc.length();
    for (size_t j = 0; j < doc.length(); j++) {
      LOG(INFO) << doc.keyAt(j).copyString() << ','
                << doc.valueAt(j).copyString();
    }
  }
}

void aql_query(const nebulas_transaction_db_ptr_t ptr) {
  auto result = ptr->aql_query(
      "for h in height sort h.block_height desc limit 1 return h.block_height");
  auto documents = result->slices().front().get("result");
  LOG(INFO) << documents;
  LOG(INFO) << documents.length();
  for (size_t i = 0; i < documents.length(); i++) {
    auto doc = documents.at(i);
    LOG(INFO) << doc.getInt();
  }
}

void transaction_reader(const nebulas_transaction_db_ptr_t ptr) {
  // std::vector<neb::transaction_info_t> txs =
  // ptr->read_transaction_simplified_from_db_with_duration(320010, 320020);
  std::vector<neb::transaction_info_t> txs =
      ptr->read_success_and_failed_transaction_from_db_with_duration(320010,
                                                                     320020);
  for (auto it = txs.begin(); it != txs.end(); it++) {
    int32_t status = it->template get<::neb::status>();
    std::string from = it->template get<::neb::from>();
    std::string to = it->template get<::neb::to>();
    std::string tx_value = it->template get<::neb::tx_value>();
    int64_t height = it->template get<::neb::height>();
    std::string timestamp = it->template get<::neb::timestamp>();
    std::string type_from = it->template get<::neb::type_from>();
    std::string type_to = it->template get<::neb::type_to>();
    std::string gas_used = it->template get<::neb::gas_used>();
    std::string gas_price = it->template get<::neb::gas_price>();
    std::string contract_address = it->template get<::neb::contract_address>();
    LOG(INFO) << status << ',' << from << ',' << to << ',' << tx_value << ','
              << height << ',' << timestamp << ',' << type_from << ','
              << type_to << ',' << gas_used << ',' << gas_price << ','
              << contract_address;
  }
}

void account_reader(const nebulas_account_db_ptr_t ac_ptr) {
  ac_ptr->set_height_address_val(320010, 320020);
  // double value = ac_ptr->get_normalized_value(100);
  // LOG(INFO) << value;
}

int main(int argc, char *argv[]) {
  nebulas_transaction_db_t tdb(STR(DB_URL), STR(DB_USER_NAME), STR(DB_PASSWORD),
                               STR(NEBULAS_DB));
  nebulas_transaction_db_ptr_t tx_ptr =
      std::make_shared<nebulas_transaction_db_t>(tdb);
  std::shared_ptr<::arangodb::fuerte::Connection> conn_ptr =
      tx_ptr->db_connection_ptr();

  nebulas_account_db_t adb(STR(DB_URL), STR(DB_USER_NAME), STR(DB_PASSWORD),
                           STR(NEBULAS_DB));
  nebulas_account_db_ptr_t ac_ptr = std::make_shared<nebulas_account_db_t>(adb);
  account_reader(ac_ptr);
  return 0;
}