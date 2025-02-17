#include "duckdb/optimizer/orca/search/Job.hpp"

namespace duckdb {

class JobFactory;
using JobFactoryPtr = std::shared_ptr<JobFactory>;

class JobFactory {
private:
    const size_t m_ulJobs;
public:
    JobFactory(const JobFactory &) = delete;

	// ctor
	JobFactory(size_t ulJobs);

	// dtor
	~JobFactory();

    // create job of specific type
	JobPtr PjCreate(Job::EJobType ejt);

	// release completed job
	void Release(JobPtr pj);

	// truncate the container for the specific job type
	void Truncate(Job::EJobType ejt);
};

}