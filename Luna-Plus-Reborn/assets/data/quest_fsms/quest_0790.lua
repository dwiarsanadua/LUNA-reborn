--[[
  Quest 790: Cheeky things
  Level Required: 57
  NPC Start: 44, NPC Complete: 44
  Rewards: EXP=318693, Gold=13316
]]

local fsm = require('fsm_engine')

local quest_790 = fsm:new({
    id = 790,
    name = "Cheeky things",
    level_required = 57,
    npc_start = 44,
    npc_complete = 44,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 790: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 790: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 790: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 35,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 790: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 790: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 790: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 790: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 790: Rewards given")
                self:give_rewards({
                    exp = 318693,
                    gold = 13316,
                    items = {
                        { item_id = 21001505, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_790