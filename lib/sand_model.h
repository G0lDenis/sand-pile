#include <deque>
#include <string>

class SandModel {
private:
    int length;
    int width;
    std::deque<std::deque<int>> sandbox;
public:
    SandModel(int l, int w, std::string filename = "");

    void LoadFromTsv(std::string& filename);

    bool CollapseSand();

    void AddUpperRow();

    void AddLowerRow();

    void AddLeftColumn();

    void AddRightColumn();

    std::deque<std::deque<int>>& GetDeque();

    friend std::ostream& operator<<(std::ostream& stream, const SandModel& model);
};
