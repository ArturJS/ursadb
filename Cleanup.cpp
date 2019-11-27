#include <array>
#include <fstream>
#include <iostream>
#include <list>
#include <stack>
#include <utility>
#include <variant>
#include <vector>
#include <zmq.hpp>

#include "Command.h"
#include "Database.h"
#include "DatasetBuilder.h"
#include "OnDiskDataset.h"
#include "QueryParser.h"

static bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

int main(int argc, char *argv[]) {
    bool dry_run = true;

    if (argc != 2 && argc != 3) {
        printf("Usage: ursadb_cleanup [db_file]\n");
        return 1;
    }

    if (argc == 3) {
        if (strcmp(argv[1], "--confirm") == 0) {
            dry_run = false;
        } else {
            printf("Invalid parameters.\n");
            return 1;
        }
    }

    std::string db_name(argv[argc-1]);
    Database db(db_name);

    std::set<std::string> db_files;
    db_files.insert(db.get_name());

    for (OnDiskDataset *workingSet : db.working_sets()) {
        db_files.insert(workingSet->get_name());

        db_files.insert("files." + workingSet->get_name());
        db_files.insert("ndx.files." + workingSet->get_name());

        for (const OnDiskIndex &index : workingSet->get_indexes()) {
            db_files.insert(index.get_fname());
        }
    }

    fs::directory_iterator end;

    for (fs::directory_iterator dir(db.get_base()); dir != end; ++dir) {
        if (fs::is_regular_file(dir->path())) {
            fs::path fn = dir->path().filename();

            if (endsWith(fn.string(), "." + db.get_name().string())) {
                if (db_files.find(fn.string()) == db_files.end()) {
                    std::cout << "unlinked file " << fn.string();

                    if (!dry_run) {
                        if (fs::remove(dir->path())) {
                            std::cout << " deleted succesfully";
                        } else {
                            std::cout << " failed to delete";
                        }
                    }

                    std::cout << std::endl;
                } else {
                    std::cout << "required file " << fn.string() << std::endl;
                }
            }
        }
    }

    if (dry_run) {
        std::cout << "in order to really remove unlinked files, execute:" << std::endl;
        std::cout << "    ursadb_cleanup --confirm " + db_name << std::endl;
        std::cout << "be warned that executing this command is not safe it UrsaDB is running" << std::endl;
    }

    return 0;
}
