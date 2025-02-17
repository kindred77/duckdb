#pragma once

namespace duckdb {

class ReqdProp;
using ReqdPropPtr = std::shared_ptr<ReqdProp>;

class ReqdProp {
private:

public:
	// types of required properties
	enum EPropType {
		EptRelational,
		EptPlan,

		EptSentinel
	};

    ReqdProp(const ReqdProp &) = delete;

	// ctor
	ReqdProp();

	// dtor
	~ReqdProp();

	// is it a relational property?
	virtual bool
	FRelational() const {
		return false;
	}

	// is it a plan property?
	virtual bool
	FPlan() const {
		return false;
	}

};

}