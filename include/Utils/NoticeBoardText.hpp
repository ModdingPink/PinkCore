#pragma once

#include <string>
namespace PinkCore
{
	class NoticeBoardText
	{
		NoticeBoardText() = delete;

		public:
			/// @brief downloads whatever text from the noticeboardURL in static-defines.h
			static void Download();

			/// @brief gets you the text that was downloaded
			static const std::string& get_text();

		private:
			/// @brief the text that was downloaded
			static std::string text;
	};
}