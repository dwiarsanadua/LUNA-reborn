--[[
  Quest 123: Dragon's Test
  Level Required: 1
  NPC Start: 27, NPC Complete: 27
  Prerequisites: [122]
  Rewards: EXP=19326, Gold=3040
]]

local fsm = require('fsm_engine')

local quest_123 = fsm:new({
    id = 123,
    name = "Dragon's Test",
    level_required = 1,
    npc_start = 27,
    npc_complete = 27,
    prerequisites = {122},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 123: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 123: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 123: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 52,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 123: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 11,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 123: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 123: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 123: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 123: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 123: Rewards given")
                self:give_rewards({
                    exp = 19326,
                    gold = 3040,
                    items = {
                        { item_id = 58, count = 5 },
                        { item_id = 21000006, count = 5 },
                        { item_id = 59, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_123