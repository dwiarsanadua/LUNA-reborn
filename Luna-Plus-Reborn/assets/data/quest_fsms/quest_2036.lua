--[[
  Quest 2036: [Daily] Squishing the Black Widow Clan
  Level Required: 86
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=1752972
]]

local fsm = require('fsm_engine')

local quest_2036 = fsm:new({
    id = 2036,
    name = "[Daily] Squishing the Black Widow Clan",
    level_required = 86,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2036: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2036: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2036: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 279,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2036: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2036: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2036: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2036: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2036: Rewards given")
                self:give_rewards({
                    exp = 1752972,
                    items = {
                        { item_id = 30000835, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2036