--[[
  Quest 712: Inappropriate Material
  Level Required: 65
  NPC Start: 46, NPC Complete: 46
  Prerequisites: [708]
  Rewards: EXP=987718
]]

local fsm = require('fsm_engine')

local quest_712 = fsm:new({
    id = 712,
    name = "Inappropriate Material",
    level_required = 65,
    npc_start = 46,
    npc_complete = 46,
    prerequisites = {708},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 712: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 712: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 712: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 712: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 712: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 712: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 712: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 712: Rewards given")
                self:give_rewards({
                    exp = 987718,
                    items = {
                        { item_id = 357, count = 1 },
                        { item_id = 357, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_712