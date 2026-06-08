--[[
  Quest 486: [Special][Quest] Let's try to fish some more
  Level Required: 0
  NPC Start: 64, NPC Complete: 121
  Prerequisites: [485]
  Rewards: EXP=93
]]

local fsm = require('fsm_engine')

local quest_486 = fsm:new({
    id = 486,
    name = "[Special][Quest] Let's try to fish some more",
    level_required = 0,
    npc_start = 64,
    npc_complete = 121,
    prerequisites = {485},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 486: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 486: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 486: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 486: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 121,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 486: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 486: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 121,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 486: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 486: Rewards given")
                self:give_rewards({
                    exp = 93,
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

return quest_486