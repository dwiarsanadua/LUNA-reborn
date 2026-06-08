--[[
  Quest 27: Missing
  Level Required: 38
  NPC Start: 29, NPC Complete: 37
  Rewards: EXP=51033, Gold=550
]]

local fsm = require('fsm_engine')

local quest_27 = fsm:new({
    id = 27,
    name = "Missing",
    level_required = 38,
    npc_start = 29,
    npc_complete = 37,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 27: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 27: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 27: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 27: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 27: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 27: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 27: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 27: Rewards given")
                self:give_rewards({
                    exp = 51033,
                    gold = 550,
                    items = {
                        { item_id = 21000095, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_27