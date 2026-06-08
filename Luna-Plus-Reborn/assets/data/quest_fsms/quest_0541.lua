--[[
  Quest 541: [Repeat][Quest] Altered Seeds
  Level Required: 150
  NPC Start: 99, NPC Complete: 99
  Prerequisites: [540]
  Rewards: EXP=5000000
]]

local fsm = require('fsm_engine')

local quest_541 = fsm:new({
    id = 541,
    name = "[Repeat][Quest] Altered Seeds",
    level_required = 150,
    npc_start = 99,
    npc_complete = 99,
    prerequisites = {540},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 541: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 541: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 541: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 273,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 541: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 276,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 541: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 541: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 541: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 541: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 541: Rewards given")
                self:give_rewards({
                    exp = 5000000,
                    items = {
                        { item_id = 337, count = 1 },
                        { item_id = 337, count = 1 },
                        { item_id = 337, count = 1 },
                        { item_id = 30000051, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_541