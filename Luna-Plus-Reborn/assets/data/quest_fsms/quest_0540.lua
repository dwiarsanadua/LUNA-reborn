--[[
  Quest 540: [Repeat][Quest] Tainted Seeds
  Level Required: 150
  NPC Start: 99, NPC Complete: 99
  Rewards: EXP=5000000
]]

local fsm = require('fsm_engine')

local quest_540 = fsm:new({
    id = 540,
    name = "[Repeat][Quest] Tainted Seeds",
    level_required = 150,
    npc_start = 99,
    npc_complete = 99,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 540: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 540: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 540: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 273,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 540: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 276,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 540: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 540: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 540: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 540: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 540: Rewards given")
                self:give_rewards({
                    exp = 5000000,
                    items = {
                        { item_id = 336, count = 1 },
                        { item_id = 336, count = 1 },
                        { item_id = 336, count = 200 },
                        { item_id = 30000303, count = 1 },
                        { item_id = 30000313, count = 1 },
                        { item_id = 21000271, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_540