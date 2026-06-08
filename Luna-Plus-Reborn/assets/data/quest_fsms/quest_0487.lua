--[[
  Quest 487: [Special][Quest] 3 Crucian Carps!
  Level Required: 0
  NPC Start: 64, NPC Complete: 121
  Prerequisites: [486]
  Rewards: EXP=206
]]

local fsm = require('fsm_engine')

local quest_487 = fsm:new({
    id = 487,
    name = "[Special][Quest] 3 Crucian Carps!",
    level_required = 0,
    npc_start = 64,
    npc_complete = 121,
    prerequisites = {486},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 487: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 121,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 487: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 487: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 121,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 487: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 487: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 487: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 121,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 487: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 487: Rewards given")
                self:give_rewards({
                    exp = 206,
                    items = {
                        { item_id = 21000313, count = 50 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_487