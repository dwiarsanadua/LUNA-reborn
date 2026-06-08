--[[
  Quest 2073: [Daily] The Temple of Greed(Beginner)
  Level Required: 90
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=3435786
]]

local fsm = require('fsm_engine')

local quest_2073 = fsm:new({
    id = 2073,
    name = "[Daily] The Temple of Greed(Beginner)",
    level_required = 90,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2073: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2073: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2073: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 643,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2073: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2073: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2073: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2073: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2073: Rewards given")
                self:give_rewards({
                    exp = 3435786,
                    items = {
                        { item_id = 30000836, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2073