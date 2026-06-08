--[[
  Quest 397: [repeatable][quest] Giant spiders
  Level Required: 63
  NPC Start: 93, NPC Complete: 93
  Prerequisites: [396]
  Rewards: EXP=692952
]]

local fsm = require('fsm_engine')

local quest_397 = fsm:new({
    id = 397,
    name = "[repeatable][quest] Giant spiders",
    level_required = 63,
    npc_start = 93,
    npc_complete = 93,
    prerequisites = {396},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 397: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 397: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 397: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 249,
                    count = 70,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 397: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 397: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 397: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 397: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 397: Rewards given")
                self:give_rewards({
                    exp = 692952,
                })
            end,
            transitions = {},
        },
    },
})

return quest_397