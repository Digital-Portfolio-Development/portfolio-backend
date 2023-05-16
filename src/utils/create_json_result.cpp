#include "utils.hpp"

namespace portfolio::utils {
userver::formats::json::Value CreateJsonResult(
    userver::storages::postgres::TypedResultSet<
        UserTuple, userver::storages::postgres::RowTag>& iteration) {
    userver::formats::json::ValueBuilder json_res;

    for (auto row : iteration) {
        userver::formats::json::ValueBuilder tmp_res;

        auto [user_id, name, last_name, username, email, phone_number, password,
              profession, key_skills, about, country, city, user_avatar, banner,
              created_at, updated_at] = row;
        tmp_res["user_id"] = user_id;
        tmp_res["name"] = name;
        tmp_res["last_name"] = last_name;
        tmp_res["username"] = username;
        tmp_res["email"] = email;
        tmp_res["phone_number"] = phone_number;
        tmp_res["password"] = password;
        tmp_res["profession"] = profession;
        tmp_res["key_skills"] = key_skills;
        tmp_res["about"] = about;
        tmp_res["country"] = country;
        tmp_res["city"] = city;
        tmp_res["avatar"] = user_avatar;
        tmp_res["banner"] = banner;
        tmp_res["created_at"] = created_at;
        tmp_res["updated_at"] = updated_at;

        json_res.PushBack(std::move(tmp_res));
    }

    return json_res.ExtractValue();
}

userver::formats::json::Value CreateJsonResult(
    userver::storages::postgres::TypedResultSet<
        ProjectTuple, userver::storages::postgres::RowTag>& iteration) {
    userver::formats::json::ValueBuilder json_res;

    for (auto row : iteration) {
        userver::formats::json::ValueBuilder tmp_res;

        auto [project_id, user_id, name, short_description, full_description,
              tags, link, avatar, priority, visibility, views, created_at,
              updated_at, username, user_avatar] = row;
        tmp_res["project_id"] = project_id;
        tmp_res["user_id"] = user_id;
        tmp_res["name"] = name;
        tmp_res["short_description"] = short_description;
        tmp_res["full_description"] = full_description;
        tmp_res["tags"] = tags;
        tmp_res["link"] = link;
        tmp_res["avatar"] = avatar;
        tmp_res["priority"] = priority;
        tmp_res["visibility"] = visibility;
        tmp_res["views"] = views;
        tmp_res["created_at"] = created_at;
        tmp_res["updated_at"] = updated_at;
        tmp_res["username"] = username;
        tmp_res["user_avatar"] = user_avatar;

        json_res.PushBack(std::move(tmp_res));
    }

    return json_res.ExtractValue();
}

userver::formats::json::Value CreateJsonResult(
    userver::storages::postgres::TypedResultSet<
        PostTuple, userver::storages::postgres::RowTag>& iteration) {
    userver::formats::json::ValueBuilder json_res;

    for (auto row : iteration) {
        userver::formats::json::ValueBuilder tmp_res;

        auto [post_id, user_id, name, text, media, tags, created_at, updated_at,
              username, user_avatar] = row;
        tmp_res["post_id"] = post_id;
        tmp_res["user_id"] = user_id;
        tmp_res["name"] = name;
        tmp_res["text"] = text;
        tmp_res["media"] = media;
        tmp_res["tags"] = tags;
        tmp_res["created_at"] = created_at;
        tmp_res["updated_at"] = updated_at;
        tmp_res["username"] = username;
        tmp_res["user_avatar"] = user_avatar;

        json_res.PushBack(std::move(tmp_res));
    }

    return json_res.ExtractValue();
}

userver::formats::json::Value CreateJsonResult(
    userver::storages::postgres::TypedResultSet<
        CommentTuple, userver::storages::postgres::RowTag>& iteration) {
    userver::formats::json::ValueBuilder json_res;

    for (auto row : iteration) {
        userver::formats::json::ValueBuilder tmp_res;

        auto [comment_id, target_id, user_id, target_type, text, media,
              created_at, updated_at, username, user_avatar] = row;
        tmp_res["comment_id"] = comment_id;
        tmp_res["target_id"] = target_id;
        tmp_res["user_id"] = user_id;
        tmp_res["target_type"] = target_type;
        tmp_res["text"] = text;
        tmp_res["media"] = media;
        tmp_res["created_at"] = created_at;
        tmp_res["updated_at"] = updated_at;
        tmp_res["username"] = username;
        tmp_res["user_avatar"] = user_avatar;

        json_res.PushBack(std::move(tmp_res));
    }

    return json_res.ExtractValue();
}

userver::formats::json::Value CreateJsonResult(
    userver::storages::postgres::TypedResultSet<
        LikeTuple, userver::storages::postgres::RowTag>& iteration) {
    userver::formats::json::ValueBuilder json_res;

    for (auto row : iteration) {
        userver::formats::json::ValueBuilder tmp_res;

        auto [target_id, user_id, target_type, created_at, username] = row;
        json_res["target_id"] = target_id;
        json_res["user_id"] = user_id;
        json_res["target_type"] = target_type;
        json_res["created_at"] = created_at;
        json_res["username"] = username;

        json_res.PushBack(std::move(tmp_res));
    }

    return json_res.ExtractValue();
}
}  // namespace portfolio::utils