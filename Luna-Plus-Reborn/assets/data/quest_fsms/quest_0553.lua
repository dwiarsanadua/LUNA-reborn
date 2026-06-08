--[[
  Quest 553: [Repeat][Quest] Tainted Dream World
  Level Required: 1
  NPC Start: 142, NPC Complete: 142
  Prerequisites: [552]
]]

local fsm = require('fsm_engine')

local quest_553 = fsm:new({
    id = 553,
    name = "[Repeat][Quest] Tainted Dream World",
    level_required = 1,
    npc_start = 142,
    npc_complete = 142,
    prerequisites = {552},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 553: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 142,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 553: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 553: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 418,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 553: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 142,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 553: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 553: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 142,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 553: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 553: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000625, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_553