--[[
  Quest 548: [Repeat][Quest] Magical Horns
  Level Required: 150
  NPC Start: 98, NPC Complete: 98
  Rewards: EXP=1000000
]]

local fsm = require('fsm_engine')

local quest_548 = fsm:new({
    id = 548,
    name = "[Repeat][Quest] Magical Horns",
    level_required = 150,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 548: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 548: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 548: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 271,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 548: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 548: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 548: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 548: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 548: Rewards given")
                self:give_rewards({
                    exp = 1000000,
                    items = {
                        { item_id = 344, count = 20 },
                        { item_id = 30000039, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_548