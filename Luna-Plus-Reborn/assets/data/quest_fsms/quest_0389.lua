--[[
  Quest 389: [Quest] Neutralizing this poison
  Level Required: 53
  NPC Start: 92, NPC Complete: 92
  Rewards: EXP=303980, Gold=15000
]]

local fsm = require('fsm_engine')

local quest_389 = fsm:new({
    id = 389,
    name = "[Quest] Neutralizing this poison",
    level_required = 53,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 389: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 389: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 389: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 93,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 389: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 389: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 389: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 389: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 389: Rewards given")
                self:give_rewards({
                    exp = 303980,
                    gold = 15000,
                    items = {
                        { item_id = 294, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_389