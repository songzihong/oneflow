#include "oneflow/core/framework/framework.h"
namespace oneflow {

REGISTER_USER_OP("truediv")
    .Input("x")
    .Input("y")
    .Output("z")
    .SetTensorDescInferFn([](user_op::InferContext* ctx) -> Maybe<void> {
      Shape* x_shape = ctx->Shape4ArgNameAndIndex("x", 0);
      Shape* y_shape = ctx->Shape4ArgNameAndIndex("y", 0);
      Shape* z_shape = ctx->Shape4ArgNameAndIndex("z", 0);
      CHECK(*y_shape == *x_shape);
      *z_shape = *x_shape;
      return Maybe<void>::Ok();
    })
    // .SetDataTypeInferFn([](user_op::InferContext* ctx) -> Maybe<void> {
    //   DataType* x_dtype = ctx->Dtype4ArgNameAndIndex("x", 0);
    //   DataType* y_dtype = ctx->Dtype4ArgNameAndIndex("y", 0);
    //   DataType* z_dtype = ctx->Dtype4ArgNameAndIndex("z", 0);
    //   CHECK(*y_dtype == *x_dtype);
    //   if (*x_dtype == DataType::kInt8) {
    //     *z_dtype = DataType::kFloat;
    //   } else if (*x_dtype == DataType::kInt32 || *x_dtype == DataType::kInt64) {
    //     *z_dtype = DataType::kDouble;
    //   } else {
    //     *z_dtype = *x_dtype;
    //   }
    //   return Maybe<void>::Ok();
    // })
    .SetGetSbpFn([](user_op::SbpContext* ctx) -> Maybe<void> {
      const user_op::TensorDesc& tensor_x = ctx->LogicalTensorDesc4InputArgNameAndIndex("x", 0);
      // const user_op::TensorDesc& tensor_y = ctx->LogicalTensorDesc4InputArgNameAndIndex("y", 0);
      SbpSignatureBuilder()
          .Split(ctx->inputs(), 0)
          .Split(ctx->outputs(), 0)
          .MakeSplitSignatureListBuilder(tensor_x.shape().NumAxes())
          .Build(ctx->sbp_sig_list());
      return Maybe<void>::Ok();
    });

REGISTER_USER_OP("truediv_x_grad")
    .Input("x")
    .Input("y")
    .Input("dz")
    .Output("dx")
    .SetTensorDescInferFn([](user_op::InferContext* ctx) -> Maybe<void> {
      Shape* x_shape = ctx->Shape4ArgNameAndIndex("x", 0);
      Shape* y_shape = ctx->Shape4ArgNameAndIndex("y", 0);
      Shape* dz_shape = ctx->Shape4ArgNameAndIndex("dz", 0);
      Shape* dx_shape = ctx->Shape4ArgNameAndIndex("dx", 0);
      CHECK((*y_shape == *x_shape) && (*dz_shape == *x_shape));
      *dx_shape = *x_shape;
      return Maybe<void>::Ok();
    })
    // .SetDataTypeInferFn([](user_op::InferContext* ctx) -> Maybe<void> {
    //   DataType* x_dtype = ctx->Dtype4ArgNameAndIndex("x", 0);
    //   DataType* y_dtype = ctx->Dtype4ArgNameAndIndex("y", 0);
    //   DataType* dx_dtype = ctx->Dtype4ArgNameAndIndex("dx", 0);
    //   CHECK(*y_dtype == *x_dtype);
    //   *dx_dtype = *x_dtype;
    //   return Maybe<void>::Ok();
    // })
    .SetGetSbpFn([](user_op::SbpContext* ctx) -> Maybe<void> {
      const user_op::TensorDesc& tensor = ctx->LogicalTensorDesc4InputArgNameAndIndex("dz", 0);
      SbpSignatureBuilder()
          .Split(ctx->inputs(), 0)
          .Split(ctx->outputs(), 0)
          .MakeSplitSignatureListBuilder(tensor.shape().NumAxes())
          .Build(ctx->sbp_sig_list());
      return Maybe<void>::Ok();
    });

REGISTER_USER_OP("truediv_y_grad")
    .Input("x")
    .Input("y")
    .Input("dz")
    .Output("dy")
    .SetTensorDescInferFn([](user_op::InferContext* ctx) -> Maybe<void> {
      Shape* x_shape = ctx->Shape4ArgNameAndIndex("x", 0);
      Shape* y_shape = ctx->Shape4ArgNameAndIndex("y", 0);
      Shape* dz_shape = ctx->Shape4ArgNameAndIndex("dz", 0);
      Shape* dy_shape = ctx->Shape4ArgNameAndIndex("dy", 0);
      CHECK((*y_shape == *x_shape) && (*dz_shape == *x_shape));
      *dy_shape = *y_shape;
      return Maybe<void>::Ok();
    })
    // .SetDataTypeInferFn([](user_op::InferContext* ctx) -> Maybe<void> {
    //   DataType* x_dtype = ctx->Dtype4ArgNameAndIndex("x", 0);
    //   DataType* y_dtype = ctx->Dtype4ArgNameAndIndex("y", 0);
    //   DataType* dy_dtype = ctx->Dtype4ArgNameAndIndex("dy", 0);
    //   CHECK(*y_dtype == *x_dtype);
    //   *dy_dtype = *x_dtype;
    //   return Maybe<void>::Ok();
    // })
    .SetGetSbpFn([](user_op::SbpContext* ctx) -> Maybe<void> {
      const user_op::TensorDesc& tensor = ctx->LogicalTensorDesc4InputArgNameAndIndex("dz", 0);
      SbpSignatureBuilder()
          .Split(ctx->inputs(), 0)
          .Split(ctx->outputs(), 0)
          .MakeSplitSignatureListBuilder(tensor.shape().NumAxes())
          .Build(ctx->sbp_sig_list());
      return Maybe<void>::Ok();
    });

REGISTER_USER_OP_GRAD("truediv").SetGenBackwardOpConfFn([](const user_op::UserOpWrapper& op,
                                                           user_op::AddOpFn AddOp) {
  if (op.NeedGenGradTensor4OpInput("x", 0)) {
    user_op::UserOpConfWrapperBuilder builder(op.op_name() + "_x_grad");
    user_op::UserOpConfWrapper truediv_grad_op =
        builder.Op("truediv_x_grad")
            .Input("x", op.input("x", 0))
            .Input("y", op.input("y", 0))
            .Input("dz", op.GetGradTensorWithOpOutput("z", 0))
            .Output("dx")
            .Build();
    op.BindGradTensorWithOpInput(truediv_grad_op.output("dx", 0), "x", 0);
    AddOp(truediv_grad_op);
  }
  if (op.NeedGenGradTensor4OpInput("y", 0)) {
    user_op::UserOpConfWrapperBuilder builder(op.op_name() + "_y_grad");
    user_op::UserOpConfWrapper truediv_grad_op =
        builder.Op("truediv_y_grad")
            .Input("x", op.input("x", 0))
            .Input("y", op.input("y", 0))
            .Input("dz", op.GetGradTensorWithOpOutput("z", 0))
            .Output("dy")
            .Build();
    op.BindGradTensorWithOpInput(truediv_grad_op.output("dy", 0), "y", 0);
    AddOp(truediv_grad_op);
  }
});
}  // namespace oneflow