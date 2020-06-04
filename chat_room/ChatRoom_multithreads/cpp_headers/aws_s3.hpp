#ifndef _AWS_S3_HPP_
#define _AWS_S3_HPP_

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/CreateBucketRequest.h>
#define getObjectBuffer 5120
using namespace std;

// //snippet-start:[s3.cpp.list_buckets.inc]
// #include <aws/core/Aws.h>
// #include <aws/s3/S3Client.h>
// #include <aws/s3/model/Bucket.h>
// //snippet-end:[s3.cpp.list_buckets.inc]

// /**
//  * List your Amazon S3 buckets.
//  */
// int main(int argc, char** argv)
// {
//     Aws::SDKOptions options;
//     Aws::InitAPI(options);
//     {
//         // snippet-start:[s3.cpp.list_buckets.code]
//         Aws::S3::S3Client s3_client;
//         auto outcome = s3_client.ListBuckets();

//         if (outcome.IsSuccess())
//         {
//             std::cout << "Your Amazon S3 buckets:" << std::endl;

//             Aws::Vector<Aws::S3::Model::Bucket> bucket_list =
//                 outcome.GetResult().GetBuckets();

//             for (auto const &bucket : bucket_list)
//             {
//                 std::cout << "  * " << bucket.GetName() << std::endl;
//             }
//         }
//         else
//         {
//             std::cout << "ListBuckets error: "
//                 << outcome.GetError().GetExceptionName() << " - "
//                 << outcome.GetError().GetMessage() << std::endl;
//         }
//         // snippet-end:[s3.cpp.list_buckets.code]
//     }
//     Aws::ShutdownAPI(options);
// }

// //snippet-start:[s3.cpp.delete_bucket.inc]
// #include <aws/core/Aws.h>
// #include <aws/s3/S3Client.h>
// #include <aws/s3/model/DeleteBucketRequest.h>
// //snippet-end:[s3.cpp.delete_bucket.inc]

// /**
//  * Delete an Amazon S3 bucket.
//  *
//  * ++ Warning ++ This code will actually delete the bucket that you specify!
//  */
// int main(int argc, char** argv)
// {
//     if (argc < 2)
//     {
//         std::cout << "delete_bucket - delete an S3 bucket" << std::endl
//             << "\nUsage:" << std::endl
//             << "  delete_bucket <bucket> [region]" << std::endl
//             << "\nWhere:" << std::endl
//             << "  bucket - the bucket to delete" << std::endl
//             << "  region - AWS region for the bucket" << std::endl
//             << "           (optional, default: us-east-1)" << std::endl
//             << "\nNote! This will actually delete the bucket that you specify!"
//             << std::endl
//             << "\nExample:" << std::endl
//             << "  delete_bucket testbucket\n" << std::endl << std::endl;
//         exit(1);
//     }

//     Aws::SDKOptions options;
//     Aws::InitAPI(options);
//     {
//         const Aws::String bucket_name = argv[1];
//         const Aws::String user_region = (argc >= 3) ? argv[2] : "us-east-1";

//         std::cout << "Deleting S3 bucket: " << bucket_name << std::endl;

//         // snippet-start:[s3.cpp.delete_bucket.code]
//         Aws::Client::ClientConfiguration config;
//         config.region = user_region;
//         Aws::S3::S3Client s3_client(config);

//         Aws::S3::Model::DeleteBucketRequest bucket_request;
//         bucket_request.SetBucket(bucket_name);

//         auto outcome = s3_client.DeleteBucket(bucket_request);

//         if (outcome.IsSuccess())
//         {
//             std::cout << "Done!" << std::endl;
//         }
//         else
//         {
//             std::cout << "DeleteBucket error: "
//                 << outcome.GetError().GetExceptionName() << " - "
//                 << outcome.GetError().GetMessage() << std::endl;
//         }
//         // snippet-end:[s3.cpp.delete_bucket.code]
//     }
//     Aws::ShutdownAPI(options);
// }

// //snippet-end:[s3.cpp.create_bucket.inc]
namespace awsS3
{
        bool create_bucket(const Aws::String &bucket_name,
                           const Aws::S3::Model::BucketLocationConstraint &region = Aws::S3::Model::BucketLocationConstraint::us_east_1)
        {
                // Set up the request
                Aws::S3::Model::CreateBucketRequest request;
                request.SetBucket(bucket_name);

                // Is the region other than us-east-1 (N. Virginia)?
                if (region != Aws::S3::Model::BucketLocationConstraint::us_east_1)
                {
                        // Specify the region as a location constraint
                        Aws::S3::Model::CreateBucketConfiguration bucket_config;
                        bucket_config.SetLocationConstraint(region);
                        request.SetCreateBucketConfiguration(bucket_config);
                }

                // Create the bucket
                Aws::S3::S3Client s3_client;
                auto outcome = s3_client.CreateBucket(request);
                if (!outcome.IsSuccess())
                {
                        auto err = outcome.GetError();
                        std::cout << "ERROR: CreateBucket: " << err.GetExceptionName() << ": " << err.GetMessage() << std::endl;
                        return false;
                }
                return true;
        }

        inline bool file_exists(const std::string &name)
        {
                struct stat buffer;
                return (stat(name.c_str(), &buffer) == 0);
        }

        bool put_s3_object(const Aws::String &s3_bucket_name,
                           const Aws::String &s3_object_name,
                           const std::string &file_name,
                           const Aws::String &region = "")
        {
                // Verify file_name exists
                if (!file_exists(file_name))
                {
                        std::cout << "ERROR: NoSuchFile: The specified file does not exist"
                                  << std::endl;
                        return false;
                }

                // If region is specified, use it
                Aws::Client::ClientConfiguration clientConfig;
                if (!region.empty())
                        clientConfig.region = region;

                // Set up request
                // snippet-start:[s3.cpp.put_object.code]
                Aws::S3::S3Client s3_client(clientConfig);
                Aws::S3::Model::PutObjectRequest object_request;

                object_request.SetBucket(s3_bucket_name);
                object_request.SetKey(s3_object_name);
                const std::shared_ptr<Aws::IOStream> input_data =
                    Aws::MakeShared<Aws::FStream>("SampleAllocationTag",
                                                  file_name.c_str(),
                                                  std::ios_base::in | std::ios_base::binary);
                object_request.SetBody(input_data);

                // Put the object
                auto put_object_outcome = s3_client.PutObject(object_request);
                if (!put_object_outcome.IsSuccess())
                {
                        auto error = put_object_outcome.GetError();
                        std::cout << "ERROR: " << error.GetExceptionName() << ": "
                                  << error.GetMessage() << std::endl;
                        return false;
                }
                return true;
                // snippet-end:[s3.cpp.put_object.code]
        }

        /**
 * Get an object from an Amazon S3 bucket.
 */
        string get_object(const Aws::String &bucket_name, const Aws::String &object_name)
        {
                // Set up the request
                Aws::S3::S3Client s3_client;
                Aws::S3::Model::GetObjectRequest object_request;
                object_request.SetBucket(bucket_name);
                object_request.SetKey(object_name);

                // Get the object
                auto get_object_outcome = s3_client.GetObject(object_request);
                if (get_object_outcome.IsSuccess())
                {
                        // Get an Aws::IOStream reference to the retrieved file
                        auto &retrieved_file = get_object_outcome.GetResultWithOwnership().GetBody();

                        //#if 1
                        // Output the first line of the retrieved text file
                        char file_data[getObjectBuffer] = {0};
                        retrieved_file.getline(file_data, getObjectBuffer - 1);
                        //std::cout << file_data << std::endl;
                        string rt(file_data);
                        return file_data;

                        // #else
                        //                 // Alternatively, read the object's contents and write to a file
                        //                 const char *filename = "/PATH/FILE_NAME";
                        //                 std::ofstream output_file(filename, std::ios::binary);
                        //                 output_file << retrieved_file.rdbuf();
                        // #endif
                }
                else
                {
                        auto error = get_object_outcome.GetError();
                        std::cout << "ERROR: " << error.GetExceptionName() << ": "
                                  << error.GetMessage() << std::endl;
                        string rt = "";
                        return rt;
                }
                // snippet-end:[s3.cpp.get_object.code]
        }

        /**
 * Delete an object from an Amazon S3 bucket.
 *
 * ++ warning ++ This will actually delete the named object!
 */
        bool delete_object(const Aws::String &bucket_name, const Aws::String key_name)
        {
                //std::cout << "Deleting" << key_name << " from S3 bucket: " << bucket_name << std::endl;
                //cout << bucket_name << " ||| " << key_name << endl;
                Aws::S3::S3Client s3_client;
                Aws::S3::Model::DeleteObjectRequest object_request;
                object_request.WithBucket(bucket_name).WithKey(key_name);
                auto delete_object_outcome = s3_client.DeleteObject(object_request);
                return (delete_object_outcome.IsSuccess());
        }

} // namespace awsS3
#endif