#pragma once

#include <string>
namespace PinkCore
{
	class DonationText
	{
		DonationText() = delete;

		public:
			/// @brief downloads whatever text from the donationURL in static-defines.h
			static void Download();

			/// @brief gets you the text that was downloaded
			static const std::string& get_text();

		private:
			/// @brief the text that was downloaded
			static std::string text;
	};
}