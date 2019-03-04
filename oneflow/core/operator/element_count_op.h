#ifndef ONEFLOW_CORE_OPERATOR_ELEMENT_COUNT_OP_H_
#define ONEFLOW_CORE_OPERATOR_ELEMENT_COUNT_OP_H_

#include "oneflow/core/operator/operator.h"

namespace oneflow {

class ElementCountOp final : public Operator {
 public:
  OF_DISALLOW_COPY_AND_MOVE(ElementCountOp);
  ElementCountOp() = default;
  ~ElementCountOp() override = default;

  void InitFromOpConf() override;
  const PbMessage& GetCustomizedConf() const override;
  void InferBlobDescs(std::function<BlobDesc*(const std::string&)> GetBlobDesc4BnInOp,
                      const ParallelContext* parallel_ctx) const override;

 private:
  bool IsInputBlobAllowedModelSplit(const std::string& ibn) const override { return true; }
};

}  // namespace oneflow

#endif  // ONEFLOW_CORE_OPERATOR_ELEMENT_COUNT_OP_H_
