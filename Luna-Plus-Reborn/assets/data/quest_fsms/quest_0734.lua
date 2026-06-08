--[[
  Quest 734: [Quest]Universal Tool Materials
  Level Required: 56
  NPC Start: 117, NPC Complete: 406
  Prerequisites: [733]
  Rewards: EXP=490174
]]

local fsm = require('fsm_engine')

local quest_734 = fsm:new({
    id = 734,
    name = "[Quest]Universal Tool Materials",
    level_required = 56,
    npc_start = 117,
    npc_complete = 406,
    prerequisites = {733},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 734: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 734: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 734: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 395,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 734: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 734: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 406,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 734: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 734: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 406,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 734: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 734: Rewards given")
                self:give_rewards({
                    exp = 490174,
                    items = {
                        { item_id = 361, count = 1 },
                        { item_id = 361, count = 1 },
                        { item_id = 374, count = 1 },
                        { item_id = 374, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_734