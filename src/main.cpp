#include<iostream>
#include<vector>
#include<string>
#include<queue>
#include<assert.h>
#include<random>
#include<fstream>
#include<algorithm>
#include<chrono>


using std::cout;

struct Point {
	int x, y;
};


struct FormsScore {
	std::string form;
	int score;
};

using WeightVec = std::vector<int>;


static const int FORMS_SIZE = 24;
static const FormsScore Forms[FORMS_SIZE] = {
	{"11111",1e5},
	{"*1111*",1e4},{"01111*",3e3},{"1*111*",3e3},{"11*11",3e3},
	{"*111*",1e3},{"0111*",3e2},{"*1*11*",3e2},
	{"*11*",100},{"011*",50},
	{"*1*",10},{"*10",1},
	{"00000",-1e5},
	{"*0000*",-1e4},{"10000*",-3e3},{"0*000*",-3e3},{"00*00",-3e3},
	{"*000*",-1e3},{"1000*",-3e2},{"*0*00*",-3e2},
	{"*00*",-100},{"*001",-50},
	{"*0*",-10},{"*01",-1}
};

WeightVec MaxWeight = { 25,12,56,63,13,35,37,36,53,57,64,94,80,63,81,61,53,77,99,71,32,81,10,13, };


typedef enum {
	WHITE,
	BLACK,
	EMPTY
}PointType;

PointType operator!(PointType type) {
	if (type == EMPTY)return EMPTY;
	return type == WHITE ? BLACK : WHITE;
}

template<typename...Ts>
using Float_t = float;

template<typename F,typename...Args>
auto timeUsed(F&& func, Args&&...args)->Float_t<decltype(func(std::forward<Args>(args)...))> {
	auto beg = std::chrono::steady_clock::now();
	func(std::forward<Args>(args)...);
	auto end = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - beg).count();
}


class ChessBoarder {
public:
	using Board_Ty = std::vector<std::vector<PointType>>;
private:
	inline static const int dirx[4] = { 0,-1,-1,-1 };
	inline static const int diry[4] = { -1,-1,0,1 };
	int MaxChesses;
	Board_Ty m_board;
	std::vector<std::vector<int>> m_exports;
	int m_chesses;
public:
	const int WIDTH, HEIGHT;
	ChessBoarder(int wid,int hgt):WIDTH(wid),HEIGHT(hgt),m_chesses(0),MaxChesses(wid*hgt){
		m_board.resize(HEIGHT + 1, std::vector<PointType>(WIDTH + 1, EMPTY));
		m_exports.resize(HEIGHT + 1, std::vector<int>(WIDTH + 1, 0));
	}

	void reset() {
		m_chesses = 0;
		for (int i = 1; i <= HEIGHT; i++) {
			for (int j = 1; j <= WIDTH; j++) {
				m_board[i][j] = EMPTY;
				m_exports[i][j] = 0;
			}
		}
	}

	bool full() {
		return m_chesses >= MaxChesses;
	}

	bool empty() {
		return m_chesses == 0;
	}

	bool inBoard(int x, int y)const {
		return x >= 1 && x <= HEIGHT && y >= 1 && y <= WIDTH;
	}


	bool connection(int x, int y)const {
		bool flag = false;
		for (int i = 0; i < 4; i++) {
			flag |= count(x, y, i).first == 5;
		}
		return flag;
	}

	int count_neib_same(int x, int y, PointType type)const {
		int count = 0;
		for (int i = x - 1; i <= x + 1; i++) {
			for (int j = y - 1; j <= y + 1; j++) {
				if (!inBoard(i, j))continue;
				if (i == x && y == j)continue;
				if (getType(i, j) == type)count++;
			}
		}
		return count;
	}


	std::pair<int, int> count(int x, int y,int d)const {
		PointType type = m_board[x][y];
		int count = 1, diff = 0;
		for (int i = 1; i <= 5; i++) {
			int tx = x + i * dirx[d], ty = y + i * diry[d];
			if (inBoard(tx, ty)) {
				if (m_board[tx][ty] == EMPTY)break;
				else if (m_board[tx][ty] == type) {
					count++;
				}
				else {
					diff++; break;
				}
				
			}
		}
		for (int i = 1; i <= 5; i++) {
			int tx = x - i * dirx[d], ty = y - i * diry[d];
			if (inBoard(tx, ty)) {
				if (m_board[tx][ty] == EMPTY)break;
				else if (m_board[tx][ty] == type) {
					count++;
				}
				else {
					diff++; break;
				}

			}
		}
		return std::pair<int, int>{count, diff};
	}

	PointType getType(int x, int y)const {
		return m_board[x][y];
	}

	int isExport(int x, int y)const {
		return m_exports[x][y];
	}

	void setType(int x, int y, PointType type) {
		static const int S = 1;
		int t = type == EMPTY ? -1 : 1;
		for (int i = x - S; i <= x + S; i++) {
			for (int j = y - S; j <= y + S; j++) {
				if (!inBoard(i, j))continue;
				m_exports[i][j] += t;
			}
		}
		m_board[x][y] = type;
		m_chesses += t;
	}

	Board_Ty& getBoard() {
		return m_board;
	}

	std::vector<std::vector<int>>& getExports() {
		return m_exports;
	}

	void drawBoard() {
		printf("|");
		for (int j = 0; j <= WIDTH; j++)
		{
			printf(" %2d|", j);
		}
		printf("\n");

		for (int i = 1; i <= HEIGHT; i++)
		{
			printf("|");
			for (int W = 0; W <= WIDTH; W++)
			{
				printf("---|");
			}
			printf("\n| %2d|", i);


			for (int j = 1; j <= WIDTH; j++)
			{
				char c = 0;
				switch (m_board[i][j])
				{
				case WHITE:
					c = 'W';
					break;
				case BLACK:
					c = 'B';
					break;
				case EMPTY:
					c = ' ';
					break;
				default:
					break;
				}
				printf(" %c |", c);
			}
			printf("\n");
		}
		printf("\n");
	}
};


class IPlayer {
protected:
	PointType m_type;
public:
	virtual Point select(const ChessBoarder& boarder) = 0;
	void setType(PointType type_) { 
		m_type = type_; 
	}
	PointType getType() {
		return m_type;
	}
	IPlayer(PointType type_) :m_type(type_) {}
	IPlayer():m_type(WHITE){}
};

class HumanPlayer :public IPlayer {
public:
	virtual Point select(const ChessBoarder& boarder) {
		int x, y;
		std::cout << ">>";
		std::cin >> x >> y;
		if (!boarder.inBoard(x, y) || boarder.getType(x, y) != EMPTY) {
			std::cout << "illegal postion" << std::endl;
			this->select(boarder);
		}
		return { x,y };
	}
	HumanPlayer(PointType type_) :IPlayer(type_) {}
};

class AI :public IPlayer {

protected:
	using BoardExport_Ty = std::vector<std::vector<int>>;
	inline static constexpr int INF = 0x7f7f7f7f;
	WeightVec m_weights;
	std::vector<int> m_scores;
public:
	static int eval(const ChessBoarder& boarder_, PointType type_, const std::vector<int>& scores) {
		static const int dirx[8] = { 0,-1,-1,-1,0,1,1,1 }, diry[8] = { -1,-1,0,1,1,1,0,-1 };
		int m = boarder_.HEIGHT, n = boarder_.WIDTH, val = 0;
		for (int i = 1; i <= m; i++) {
			for (int j = 1; j <= n; j++) {
				if (boarder_.getType(i, j) == EMPTY)continue;
				for (int dir = 0; dir < 8; dir++) {
					std::string form = "";
					for (int k = -4; k <= 4; k++) {
						int tx = i + dirx[dir] * k, ty = j + diry[dir] * k;
						if (!boarder_.inBoard(tx, ty))continue;
						PointType type = boarder_.getType(tx, ty);
						form += type == EMPTY ? '*' : (type == type_ ? '1' : '0');

					}
					val += eavl_form(form, scores);
				}
			}
		}
		return val;
	}

	static int eavl_form(const std::string& form, const std::vector<int>& scores) {
		int res = 0;
		for (int i = 0; i < FORMS_SIZE; i++) {
			const std::string& p = Forms[i].form;
			int val = scores[i];
			if (auto it = form.find(p); it != std::string::npos) {
				res += val;
			}
		}
		return res;
	}

	const std::vector<int>& getScores()const {
		return m_scores;
	}

	void setScores(const WeightVec& weight) {
		for (int i = 0; i < weight.size(); i++) {
			m_scores[i] = weight[i] * Forms[i].score;
		}
	}

	const WeightVec& getWeight()const {
		return m_weights;
	}

	void setWeight(const WeightVec& weight) {
		setScores(weight);
		this->m_weights = weight;
	}

	void setWeight(WeightVec&& weight) {
		setScores(weight);
		this->m_weights = weight;
	}
	AI(const WeightVec& weight,PointType type = BLACK):IPlayer(type),m_weights(weight){
		m_scores.resize(m_weights.size(), 0);
		setScores(m_weights);
	}
};

class AI1 :public AI {
private:
	inline static constexpr int MAX_DEPTH = 4;
	inline static constexpr int Time = 60;
	std::chrono::time_point<std::chrono::steady_clock> m_beginTime;
public:
	AI1(const WeightVec& weight,PointType type_ = BLACK):AI(weight,type_) {}

	int maxmin(int x, int y, ChessBoarder& boarder, int depth, int alph, int beta) {
		bool isMax = depth % 2 == 0;
		PointType type = isMax ? m_type : !m_type;
		boarder.setType(x, y, !type);
		if (boarder.connection(x, y) || depth == MAX_DEPTH) {
			int val = AI::eval(boarder, m_type, m_scores);
			boarder.setType(x, y, EMPTY);
			return val;
		}
		int m = boarder.HEIGHT, n = boarder.WIDTH;
		int val = isMax ? -INF : INF;

		auto compareFunc = [this, type, boarder](Point p1, Point p2)->bool {
			int cnt1 = boarder.count_neib_same(p1.x, p1.y, type), cnt2 = boarder.count_neib_same(p2.x, p2.y, type);
			return cnt1 < cnt2;
		};

		std::priority_queue<Point, std::vector<Point>, decltype(compareFunc)> que(compareFunc);

		for (int i = 1; i <= m; i++) {
			for (int j = 1; j <= n; j++) {
				if (boarder.getType(i, j) != EMPTY || boarder.isExport(i, j) == 0)continue;
				que.push({ i,j });
			}
		}

		while (!que.empty()) {
			Point cur = que.top(); que.pop();
			int t = maxmin(cur.x, cur.y, boarder, depth + 1, alph, beta);
			if (isMax) {
				alph = std::max(alph, t);
				val = std::max(val, t);
			}
			else {
				beta = std::min(beta, t);
				val = std::min(val, t);
			}
			if (alph > beta) {
				break;
			}
		}

		boarder.setType(x, y, EMPTY);
		return val;
	}

	Point select(const ChessBoarder& boarder_) override {
		m_beginTime = std::chrono::steady_clock::now();
		int m = boarder_.HEIGHT, n = boarder_.WIDTH;
		ChessBoarder boarder = boarder_;
		Point res = Point{};

		auto compareFunc = [this, boarder](Point p1, Point p2)->bool {
			int cnt1 = boarder.count_neib_same(p1.x, p1.y, m_type), cnt2 = boarder.count_neib_same(p2.x, p2.y, m_type);
			return cnt1 < cnt2;
		};

		std::priority_queue<Point, std::vector<Point>, decltype(compareFunc)> que(compareFunc);

		for (int i = 1; i <= m; i++) {
			for (int j = 1; j <= n; j++) {
				if (boarder.getType(i, j) != EMPTY || boarder.isExport(i, j) == 0)continue;
				que.push({ i,j });
			}
		}

		if (boarder.empty())que.push({ m / 2,n / 2 });


		int alpha = -INF;

		while (!que.empty()) {
			if (int count = std::chrono::duration_cast<std::chrono::duration<int>>(std::chrono::steady_clock::now() - m_beginTime).count();count >= Time) {
				return res;
			}
			Point point = que.top(); que.pop();
			int val = maxmin(point.x, point.y, boarder, 1, alpha, INF);
			//printf("(%d,%d,%d) ", point.x, point.y, val);
			if (val > alpha) {
				alpha = val;
				res = point;
			}
		}


		//system("pause");
		return res;

	}
};

class AI2 :public AI {

public:
	Point select(const ChessBoarder& boarder_) {
		int m = boarder_.HEIGHT, n = boarder_.WIDTH;
		ChessBoarder boarder = boarder_;
		Point res = {};
		int curVal = -INF;
		std::queue<Point> que;
		std::vector<std::vector<bool>> vis(m + 1, std::vector<bool>(n + 1, false));
		que.push({ m / 2,n / 2 }), vis[m / 2][n / 2] = true;
		while (!que.empty()) {
			Point cur = que.front(); que.pop();
			if (boarder.getType(cur.x, cur.y) == EMPTY) {
				boarder.setType(cur.x, cur.y, m_type);
				int val = AI::eval(boarder, m_type, m_scores);
				boarder.setType(cur.x, cur.y, EMPTY);
				//printf("(%d,%d,%d) ", cur.x, cur.y, val);
				if (val > curVal) {
					curVal = val;
					res = cur;
				}
			}

			for (int i = cur.x - 1; i <= cur.x + 1; i++) {
				for (int j = cur.y - 1; j <= cur.y + 1; j++) {
					if (i == cur.x && j == cur.y)continue;
					if (!boarder.inBoard(i, j))continue;
					if (vis[i][j] == true)continue;
					if (boarder.isExport(i, j) == 0)continue;
					vis[i][j] = true; que.push({ i,j });
				}
			}
		}
		//printf("\n(%d,%d,%d)\n", res.x, res.y, curVal);
		//system("pause");
		return res;
	}
	AI2(const WeightVec& weight,PointType type = BLACK):AI(weight,type){}
};



class Game {

	enum class SceneKind {
		MENU,
		PLAY
	};

	struct State {
		SceneKind scene;
		bool isOver;
		PointType winner;
		int player;
	};


private:
	ChessBoarder* m_board;
	IPlayer* m_players[2] = {};
	State m_state;
	inline static State init_state = { SceneKind::MENU,false,EMPTY,0 };
	inline static State gameBegin_state = { SceneKind::PLAY,false,EMPTY,0 };
private:
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	void playChess_handle(bool isShow = true) {
		select();
		m_state.player = (m_state.player + 1) & 1;
		if (isShow) {
			system("cls");
			m_board->drawBoard();
		}
	}

	void menu_handle() {
		printf("1:human vs AI  2:human vs human  3:AI vs AI \n");
		int kind; std::cin >> kind;
		m_state = gameBegin_state;
		m_board->reset();
		if (kind == 1) {
			m_players[0] = new HumanPlayer(WHITE), m_players[1] = new AI1(MaxWeight,BLACK);
		}
		else if (kind == 2) {
			m_players[0] = new HumanPlayer(WHITE), m_players[1] = new HumanPlayer(BLACK);
		}
		else if (kind == 3) {
			m_players[0] = new AI1(MaxWeight,WHITE), m_players[1] = new AI2(MaxWeight,BLACK);
		}
	}

	void judge(int x, int y,PointType type) {
		if (m_board->connection(x, y) || m_board->full()) {
			m_state.isOver = true;
			m_state.winner = type;
		}
	}

	void select() {
		IPlayer* curPlayer = m_players[m_state.player];
		Point point = curPlayer->select(*m_board);
		m_board->setType(point.x, point.y, curPlayer->getType());
		judge(point.x,point.y,curPlayer->getType());
	}

public:
	Game(int wid = 15,int hgt = 15):m_board(new ChessBoarder(wid,hgt)),m_state(init_state) {}
	void run_game() {
		m_board->drawBoard();
		while (true) {
			if (m_state.isOver == true) {
				printf(m_state.winner == WHITE?"WHITE WIN!!\n":"BLACK WIN!!\n");
				m_state = init_state;
			}
			if (m_state.scene == SceneKind::MENU) {
				menu_handle();
			}
			else if (m_state.scene == SceneKind::PLAY) {
				playChess_handle();
			}
			
		}
	}

	AI2* against(AI2* p1, AI2* p2) {
		m_board->reset();
		p1->setType(WHITE);
		p2->setType(BLACK);
		m_players[0] = p1, m_players[1] = p2;
		m_state = gameBegin_state;
		while (!m_state.isOver) {
			playChess_handle(false);
		}
		//printf(m_state.winner == WHITE ?"WHITE WIN!!\n" : "BLACK WIN!!\n");
		return static_cast<AI2*>(m_state.winner == WHITE ? p1 : p2);
	}

	bool against(const WeightVec& w1, const WeightVec& w2) {
		bool res = false;
		AI2 p1(w1), p2(w2);
		AI2* winner = against(&p1, &p2);
		res = winner == &p1 ? true : false;
		return res;
	}
};


class Trainer {

private:
	inline static constexpr int Train_Count = 25;
	inline static const int Against_Count = 100;
	inline static constexpr float Mutation_Probility = 0.10;
	Game* m_game;
	std::mt19937 m_rng;
	using Group_Ty = std::vector<WeightVec>;

public:
	WeightVec weightGentor() {
		WeightVec result;
		static std::uniform_int_distribution<int> uni(1, 100);
		result.reserve(FORMS_SIZE);
		for (int i = 0; i < FORMS_SIZE; i++)result.push_back(uni(m_rng));
		return result;
	}


	Group_Ty normalGroup(int count) {
		Group_Ty result;
		result.reserve(count);
		for (int i = 1; i <= count; i++)result.emplace_back(weightGentor());
		return result;
	}

	float fitness(const WeightVec& weight,const Group_Ty& againstGroup) {
		int len = againstGroup.size();
		int count = 0;
		for (const WeightVec& p : againstGroup) {
			if (m_game->against(weight, p))count++;
		}
		return (float)count / (float)len;
	}

	float fitness(const WeightVec& weight) {
		Group_Ty group = normalGroup(1000);
		return fitness(weight, group);
	}

	WeightVec grouper(const Group_Ty& group) {
		int cur = 0, next = 1;
		while (next < group.size()) {
			if (m_game->against(group[cur], group[next])) {
				next++;
			}
			else {
				cur = next++;
			}
		}
		return group[cur];
	}

	Group_Ty healthyGroup(int count) {
		static const int againstCount = 100;
		Group_Ty group(count);
		
		for (WeightVec& w : group)w = grouper(normalGroup(againstCount));
		return group;
	}


	WeightVec excellen_train() {
		Group_Ty group = healthyGroup(200);
		
		for (int T = 1; T <= 20; T++) {
			printf("update:%d\n", T);
			update(group);
		}

		return grouper(group);
	}

	void update(Group_Ty& group) {
		select(group);
		hybridiz(group);
	}

	void hybridiz(Group_Ty& group) {
		int len = group.size();

		for (int i = 0; i < len - 1; i++) {
			int index = std::uniform_int_distribution<int>(i + 1, len - 1)(m_rng);
			auto children = mating(group[i], group[index]);
			float rate = std::uniform_real_distribution<float>(0.0f, 1.0f)(m_rng);
			if (rate <= Mutation_Probility) {
				muta(children.first), muta(children.second);
			}
			group.push_back(children.first), group.push_back(children.second);
		}
	}

	void select(Group_Ty& group) {
		int len = group.size();
		auto againstGroup = normalGroup(Against_Count);
		std::vector<float> scores(len);
		for (int i = 0; i < len; i++) {
			scores[i] = fitness(group[i],againstGroup);
			printf("%.2f ", scores[i]);
		}
		printf("\n");

		std::vector<float> copy_scores = scores;
		std::nth_element(copy_scores.begin(), copy_scores.begin() + Train_Count - 1, copy_scores.end(),
			[](float f1, float f2)->bool {return f1 > f2; });

		float k_score = copy_scores[Train_Count - 1];

		
		for (int i = 0; i < group.size();) {
			if (scores[i] < k_score) {
				std::swap(group[i], group.back()), group.pop_back();
				std::swap(scores[i], scores.back()), scores.pop_back();
			}
			else { i++; }
		}
		group.resize(Train_Count);

	}

	std::pair<WeightVec,WeightVec> mating(const WeightVec& w1, const WeightVec& w2) {
		int len = w1.size();
		std::pair<WeightVec, WeightVec> result{ WeightVec(len),WeightVec(len) };

		std::vector<int> seletor(len,0);
		std::uniform_int_distribution<int> uniform(0,1);
		
		for (int& num : seletor)num = uniform(m_rng);

		for (int i = 0; i < len; i++) {
			if (seletor[i]) {
				result.first[i] = w1[i];
				result.second[i] = w2[i];
			}
			else {
				result.first[i] = w2[i];
				result.second[i] = w1[i];
			}
		}

		return result;
	}

	void muta(WeightVec& weight) {
		int index = std::uniform_int_distribution<int>(0, weight.size() - 1)(m_rng);
		weight[index] = std::uniform_int_distribution<int>(1, 100)(m_rng);
	}
public:
	Trainer(Game* game):m_game(game),m_rng(std::random_device()()){}
	WeightVec train() {
		return excellen_train();
	}

};



//ÑµÁ·AI
/*
int main() {
	std::fstream fs;
	Game* game = new Game();
	Trainer* trainer = new Trainer(game);
	WeightVec weights = trainer->train();
	fs.open("data.txt", std::ios_base::app);
	for (int i = 0; i < FORMS_SIZE; i++) {
		fs << weights[i] << ',';
	}
	fs << '\n';
	fs.close();

	float rate = trainer->fitness(weights);
	printf("\n%f", rate);
	return 0;
}*/


//¿ªÊ¼ÓÎÏ·
int main() {
	Game* game = new Game();
	game->run_game();
	return 0;
}
