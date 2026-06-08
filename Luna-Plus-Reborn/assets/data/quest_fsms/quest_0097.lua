--[[
  Quest 97: Smarter Than the Average Bear
  Level Required: 20
  NPC Start: 17, NPC Complete: 17
  Rewards: EXP=8296, Gold=2085
]]

local fsm = require('fsm_engine')

local quest_97 = fsm:new({
    id = 97,
    name = "Smarter Than the Average Bear",
    level_required = 20,
    npc_start = 17,
    npc_complete = 17,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 97: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 17,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 97: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 97: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 4,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 97: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 17,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 97: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 97: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 17,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 97: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 97: Rewards given")
                self:give_rewards({
                    exp = 8296,
                    gold = 2085,
                    items = {
                        { item_id = 40, count = 1 },
                        { item_id = 40, count = 1 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_97