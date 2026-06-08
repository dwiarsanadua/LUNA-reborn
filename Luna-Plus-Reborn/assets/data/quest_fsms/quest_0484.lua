--[[
  Quest 484: [Special][Quest] Fishing is Not So Easy
  Level Required: 5
  NPC Start: 64, NPC Complete: 121
  Rewards: EXP=93
]]

local fsm = require('fsm_engine')

local quest_484 = fsm:new({
    id = 484,
    name = "[Special][Quest] Fishing is Not So Easy",
    level_required = 5,
    npc_start = 64,
    npc_complete = 121,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 484: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 484: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 484: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 484: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 121,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 484: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 484: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 121,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 484: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 484: Rewards given")
                self:give_rewards({
                    exp = 93,
                    items = {
                        { item_id = 21000312, count = 50 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_484